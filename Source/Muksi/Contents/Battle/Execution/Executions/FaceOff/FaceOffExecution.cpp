#include "Muksi/Contents/Battle/Execution/Executions/FaceOff/FaceOffExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/FaceOff/FaceOffExecutionData.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"

UFaceOffExecution::UFaceOffExecution()
{
	bPresentationOnly = true;
}

void UFaceOffExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	SourceCharacter = Context.Attacker.Get();
	TargetCharacter = Context.ExecutionTarget.Get();

	const FFaceOffExecutionData* FaceOffData = Context.GetExecutionData<FFaceOffExecutionData>();

	if (!SourceCharacter || !TargetCharacter || !FaceOffData || SourceCharacter == TargetCharacter)
	{
		FinishFaceOffExecution();
		return;
	}

	SourceMovementComponent = SourceCharacter->GetBattleMovementComponent();
	TargetMovementComponent = TargetCharacter->GetBattleMovementComponent();

	if (!SourceMovementComponent || !TargetMovementComponent)
	{
		FinishFaceOffExecution();
		return;
	}

	FVector SourceLocation = SourceCharacter->GetActorLocation();
	FVector TargetLocation = TargetCharacter->GetActorLocation();
	FVector Direction = TargetLocation - SourceLocation;
	Direction.Z = 0.0f;

	if (!Direction.Normalize())
	{
		FinishFaceOffExecution();
		return;
	}

	SourceMovementComponent->SavePresentationTransform();
	TargetMovementComponent->SavePresentationTransform();

	const FVector Midpoint = (SourceLocation + TargetLocation) * 0.5f;
	const float HalfDistance = FaceOffData->CharacterDistance * 0.5f;
	FVector SourceStageLocation = Midpoint - Direction * HalfDistance;
	FVector TargetStageLocation = Midpoint + Direction * HalfDistance;

	SourceStageLocation.Z = SourceLocation.Z;
	TargetStageLocation.Z = TargetLocation.Z;

	bSourceMovementFinished = false;
	bTargetMovementFinished = false;
	bMovementInterrupted = false;

	FMuksiBattleMovementFinished SourceFinished;
	SourceFinished.BindUObject(this, &UFaceOffExecution::HandleSourceMovementFinished);

	FMuksiBattleMovementFinished TargetFinished;
	TargetFinished.BindUObject(this, &UFaceOffExecution::HandleTargetMovementFinished);

	SourceMovementComponent->StartLinearMove(SourceStageLocation, FaceOffData->MoveDuration, SourceFinished);
	TargetMovementComponent->StartLinearMove(TargetStageLocation, FaceOffData->MoveDuration, TargetFinished);
}

void UFaceOffExecution::HandleSourceMovementFinished(bool bInterrupted)
{
	bSourceMovementFinished = true;
	bMovementInterrupted |= bInterrupted;
	TryFinishMovement();
}

void UFaceOffExecution::HandleTargetMovementFinished(bool bInterrupted)
{
	bTargetMovementFinished = true;
	bMovementInterrupted |= bInterrupted;
	TryFinishMovement();
}

void UFaceOffExecution::TryFinishMovement()
{
	if (!bSourceMovementFinished || !bTargetMovementFinished)
		return;

	if (bMovementInterrupted)
	{
		RestoreSavedTransforms();
		FinishFaceOffExecution();
		return;
	}

	FaceCharactersTowardEachOther();
	FinishFaceOffExecution();
}

void UFaceOffExecution::FaceCharactersTowardEachOther()
{
	if (!SourceCharacter || !TargetCharacter || !SourceMovementComponent || !TargetMovementComponent)
		return;

	FVector SourceDirection = TargetCharacter->GetActorLocation() - SourceCharacter->GetActorLocation();
	FVector TargetDirection = -SourceDirection;
	SourceDirection.Z = 0.0f;
	TargetDirection.Z = 0.0f;

	if (!SourceDirection.IsNearlyZero())
	{
		const float SourceYaw = SourceDirection.Rotation().Yaw + SourceMovementComponent->MovementYawOffset;
		SourceCharacter->SetActorRotation(FRotator(0.0f, SourceYaw, 0.0f));
	}

	if (!TargetDirection.IsNearlyZero())
	{
		const float TargetYaw = TargetDirection.Rotation().Yaw + TargetMovementComponent->MovementYawOffset;
		TargetCharacter->SetActorRotation(FRotator(0.0f, TargetYaw, 0.0f));
	}
}

void UFaceOffExecution::RestoreSavedTransforms()
{
	if (SourceCharacter && SourceMovementComponent && SourceMovementComponent->HasSavedPresentationTransform())
	{
		SourceCharacter->SetActorTransform(SourceMovementComponent->GetSavedPresentationTransform());
		SourceMovementComponent->ClearSavedPresentationTransform();
	}

	if (TargetCharacter && TargetMovementComponent && TargetMovementComponent->HasSavedPresentationTransform())
	{
		TargetCharacter->SetActorTransform(TargetMovementComponent->GetSavedPresentationTransform());
		TargetMovementComponent->ClearSavedPresentationTransform();
	}
}

void UFaceOffExecution::FinishFaceOffExecution()
{
	if (IsExecutionFinished())
		return;

	SourceCharacter = nullptr;
	TargetCharacter = nullptr;
	SourceMovementComponent = nullptr;
	TargetMovementComponent = nullptr;
	bSourceMovementFinished = false;
	bTargetMovementFinished = false;
	bMovementInterrupted = false;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UFaceOffExecution::GetExecutionDataStruct() const
{
	return FFaceOffExecutionData::StaticStruct();
}
