#include "Muksi/Contents/Battle/Execution/Executions/Projectile/ProjectileExecution.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Projectile/BattleProjectileActor.h"
#include "Muksi/Contents/Battle/Execution/Executions/Projectile/ProjectileExecutionData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingGroup.h"

void UProjectileExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedContext = Context;
	CachedOnFinished = MoveTemp(OnFinished);
	PendingProjectileCount = 0;

	const FProjectileExecutionData* ProjectileData = Context.GetExecutionData<FProjectileExecutionData>();
	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!ProjectileData || !StepResult || !Context.Attacker || !Context.BattleGridManager || !ProjectileData->ProjectileActorClass)
	{
		CompleteExecution();
		return;
	}

	for (const FTargetingGroup& Group : StepResult->Groups)
	{
		if (Group.PathCoords.IsEmpty() && (Group.Direction == INDEX_NONE || Group.PathRange <= 0))
			continue;

		++PendingProjectileCount;
	}

	if (PendingProjectileCount <= 0)
	{
		CompleteExecution();
		return;
	}

	for (const FTargetingGroup& Group : StepResult->Groups)
	{
		if (Group.PathCoords.IsEmpty() && (Group.Direction == INDEX_NONE || Group.PathRange <= 0))
			continue;

		if (!LaunchProjectileForGroup(Context, Group))
			HandleProjectileGroupCompleted();
	}
}

bool UProjectileExecution::LaunchProjectileForGroup(const FBattleExecutionContext& Context, const FTargetingGroup& Group)
{
	const FProjectileExecutionData* ProjectileData = Context.GetExecutionData<FProjectileExecutionData>();
	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!ProjectileData || !StepResult || !Context.Attacker || !Context.BattleGridManager)
		return false;

	if (Group.PathCoords.IsEmpty() && (Group.Direction == INDEX_NONE || Group.PathRange <= 0))
		return false;

	ABattleCharacterBase* HitTarget = nullptr;

	if (!Group.PathCoords.IsEmpty())
		HitTarget = FindHitTarget(Context, Group, Group.PathCoords.Last());

	FVector TargetLocation = FVector::ZeroVector;

	if (HitTarget)
	{
		TargetLocation = HitTarget->GetActorLocation();
	}
	else if (Group.Direction != INDEX_NONE && Group.PathRange > 0)
	{
		const FHexOffsetCoord VisualDestinationCoord = FHexGridMath::GetNeighborCoord(StepResult->Step.OriginCoord, Group.Direction, Group.PathRange);
		TargetLocation = Context.BattleGridManager->GetWorldLocationByCoord(VisualDestinationCoord);
	}
	else
	{
		const FBattleGridCell* DestinationCell = Context.BattleGridManager->GetCellByCoord(Context.GridWorldType, Group.PathCoords.Last());

		if (!DestinationCell)
			return false;

		TargetLocation = DestinationCell->WorldLocation;
	}

	UWorld* World = Context.Attacker->GetWorld();

	if (!World)
		return false;

	FTransform SpawnTransform = Context.Attacker->GetActorTransform();
	USkeletalMeshComponent* BattleSkeletalMesh = Context.Attacker->GetBattleSkeletalMesh();

	if (BattleSkeletalMesh && !ProjectileData->SpawnSocketName.IsNone() && BattleSkeletalMesh->DoesSocketExist(ProjectileData->SpawnSocketName))
		SpawnTransform = BattleSkeletalMesh->GetSocketTransform(ProjectileData->SpawnSocketName, RTS_World);

	TargetLocation.Z = SpawnTransform.GetLocation().Z;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Context.Attacker;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABattleProjectileActor* Projectile = World->SpawnActor<ABattleProjectileActor>(ProjectileData->ProjectileActorClass, SpawnTransform, SpawnParameters);

	if (!Projectile)
		return false;

	Projectile->CustomTimeDilation = Context.Attacker->CustomTimeDilation;
	Projectile->SetActorHiddenInGame(Context.Attacker->IsHidden());

	FBattleProjectileFinished ProjectileFinished;
	ProjectileFinished.BindUObject(this, &UProjectileExecution::HandleProjectileFinished, HitTarget);
	Projectile->LaunchProjectile(TargetLocation, ProjectileData->MoveSpeed, MoveTemp(ProjectileFinished));
	return true;
}

const UScriptStruct* UProjectileExecution::GetExecutionDataStruct() const
{
	return FProjectileExecutionData::StaticStruct();
}

ABattleCharacterBase* UProjectileExecution::FindHitTarget(const FBattleExecutionContext& Context, const FTargetingGroup& Group, const FHexOffsetCoord& DestinationCoord) const
{
	if (Context.ExecutionTarget && Context.ExecutionTarget != Context.Attacker)
		return Context.ExecutionTarget.Get();

	for (ABattleCharacterBase* TargetCharacter : Group.Targets)
	{
		if (TargetCharacter && TargetCharacter != Context.Attacker)
			return TargetCharacter;
	}

	if (!Context.BattleGridManager)
		return nullptr;

	const FBattleGridCell* DestinationCell = Context.BattleGridManager->GetCellByCoord(Context.GridWorldType, DestinationCoord);
	ABattleCharacterBase* DestinationCharacter = DestinationCell ? Cast<ABattleCharacterBase>(DestinationCell->OccupyingActor.Get()) : nullptr;
	return DestinationCharacter != Context.Attacker ? DestinationCharacter : nullptr;
}

void UProjectileExecution::HandleProjectileFinished(bool bInterrupted, ABattleCharacterBase* HitTarget)
{
	if (!bInterrupted && HitTarget && RequestOnHitExecutionEntries(HitTarget))
		return;

	HandleProjectileGroupCompleted();
}

bool UProjectileExecution::RequestOnHitExecutionEntries(ABattleCharacterBase* HitTarget)
{
	const FProjectileExecutionData* ProjectileData = CachedContext.GetExecutionData<FProjectileExecutionData>();

	if (!ProjectileData || ProjectileData->OnHitExecutionEntries.IsEmpty() || !CachedContext.CanRequestRuntimeExecutionEntries())
		return false;

	FBattleExecutionContext RuntimeContext = CachedContext;
	RuntimeContext.ExecutionTarget = HitTarget;

	return CachedContext.RequestRuntimeExecutionEntries.Execute(
		ProjectileData->OnHitExecutionEntries,
		RuntimeContext,
		FSimpleDelegate::CreateUObject(this, &UProjectileExecution::HandleProjectileGroupCompleted));
}

void UProjectileExecution::HandleProjectileGroupCompleted()
{
	if (IsExecutionFinished())
		return;

	PendingProjectileCount = FMath::Max(0, PendingProjectileCount - 1);

	if (PendingProjectileCount <= 0)
		CompleteExecution();
}

void UProjectileExecution::CompleteExecution()
{
	if (IsExecutionFinished())
		return;

	PendingProjectileCount = 0;
	CachedContext = FBattleExecutionContext();

	FinishExecution(CachedOnFinished);
}
