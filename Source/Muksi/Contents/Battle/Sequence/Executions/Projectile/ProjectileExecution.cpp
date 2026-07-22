#include "Muksi/Contents/Battle/Sequence/Executions/Projectile/ProjectileExecution.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Projectile/BattleProjectileActor.h"
#include "Muksi/Contents/Battle/Sequence/Executions/Projectile/ProjectileExecutionData.h"

void UProjectileExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	Super::Execute(Context, OnFinished);

	CachedContext = Context;
	CachedOnFinished = MoveTemp(OnFinished);
	ActiveProjectile = nullptr;
	PendingHitTarget = nullptr;

	const FProjectileExecutionData* ProjectileData = Context.GetExecutionData<FProjectileExecutionData>();

	if (!ProjectileData || !Context.Attacker || !Context.BattleGridManager || !ProjectileData->ProjectileActorClass || Context.TargetingResult.PathCoords.IsEmpty())
	{
		CompleteExecution();
		return;
	}

	const FIntPoint DestinationCoord = Context.TargetingResult.PathCoords.Last();
	const FBattleGridCell* DestinationCell = Context.BattleGridManager->GetCell(DestinationCoord);

	if (!DestinationCell)
	{
		CompleteExecution();
		return;
	}

	FVector TargetLocation = DestinationCell->WorldLocation;

	if (!Context.TargetingResult.TargetCharacters.IsEmpty() && Context.TargetingResult.TargetCharacters[0])
	{
		PendingHitTarget = Context.TargetingResult.TargetCharacters[0];
		TargetLocation = PendingHitTarget->GetActorLocation();
	}

	UWorld* World = Context.Attacker->GetWorld();

	if (!World)
	{
		CompleteExecution();
		return;
	}

	FTransform SpawnTransform = Context.Attacker->GetActorTransform();

	if (!ProjectileData->SpawnSocketName.IsNone())
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = Context.Attacker->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (SkeletalMeshComponent->DoesSocketExist(ProjectileData->SpawnSocketName))
			{
				SpawnTransform = SkeletalMeshComponent->GetSocketTransform(ProjectileData->SpawnSocketName, RTS_World);
			}
		}
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = Context.Attacker;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ActiveProjectile = World->SpawnActor<ABattleProjectileActor>(ProjectileData->ProjectileActorClass, SpawnTransform, SpawnParameters);

	if (!ActiveProjectile)
	{
		CompleteExecution();
		return;
	}

	FBattleProjectileFinished ProjectileFinished;
	ProjectileFinished.BindUObject(this, &UProjectileExecution::HandleProjectileFinished);

	ActiveProjectile->LaunchProjectile(TargetLocation, ProjectileData->MoveSpeed, MoveTemp(ProjectileFinished));
}

const UScriptStruct* UProjectileExecution::GetExecutionDataStruct() const
{
	return FProjectileExecutionData::StaticStruct();
}

void UProjectileExecution::HandleProjectileFinished(bool bInterrupted)
{
	ActiveProjectile = nullptr;

	if (!bInterrupted && PendingHitTarget)
	{
		RequestOnHitExecutionChain();
	}

	CompleteExecution();
}

void UProjectileExecution::RequestOnHitExecutionChain()
{
	const FProjectileExecutionData* ProjectileData = CachedContext.GetExecutionData<FProjectileExecutionData>();

	if (!ProjectileData || ProjectileData->OnHitExecutionChain.IsEmpty() || !CachedContext.CanRequestRuntimeExecutionChain())
	{
		return;
	}

	FBattleExecutionContext RuntimeContext = CachedContext;
	RuntimeContext.ExecutionTarget = PendingHitTarget;

	CachedContext.RequestRuntimeExecutionChain.Execute(ProjectileData->OnHitExecutionChain, RuntimeContext);
}

void UProjectileExecution::CompleteExecution()
{
	if (IsExecutionFinished())
	{
		return;
	}

	ActiveProjectile = nullptr;
	PendingHitTarget = nullptr;

	FinishExecution(CachedOnFinished);
}