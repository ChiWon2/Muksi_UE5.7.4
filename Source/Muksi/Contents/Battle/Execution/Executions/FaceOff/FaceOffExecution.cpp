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

	const FFaceOffExecutionData* FaceOffData = Context.GetExecutionData<FFaceOffExecutionData>();

	if (!SourceCharacter || !FaceOffData)
	{
		FinishFaceOffExecution();
		return;
	}

	TargetCharacter = ResolveTargetCharacter(Context, FaceOffData->TargetPolicy);

	if (!TargetCharacter || SourceCharacter == TargetCharacter)
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

	FVector SourceStageLocation = SourceLocation;
	FVector TargetStageLocation = TargetLocation;

	switch (FaceOffData->MoveMode)
	{
	case EFaceOffMoveMode::AttackerToTarget:
		SourceStageLocation = TargetLocation - Direction * FaceOffData->CharacterDistance;
		break;

	case EFaceOffMoveMode::TargetToAttacker:
		TargetStageLocation = SourceLocation + Direction * FaceOffData->CharacterDistance;
		break;

	case EFaceOffMoveMode::Both:
	default:
	{
		const FVector Midpoint = (SourceLocation + TargetLocation) * 0.5f;
		const float HalfDistance = FaceOffData->CharacterDistance * 0.5f;
		SourceStageLocation = Midpoint - Direction * HalfDistance;
		TargetStageLocation = Midpoint + Direction * HalfDistance;
		break;
	}
	}

	SourceStageLocation.Z = SourceLocation.Z;
	TargetStageLocation.Z = TargetLocation.Z;

	bSourceMovementFinished = FaceOffData->MoveMode == EFaceOffMoveMode::TargetToAttacker;
	bTargetMovementFinished = FaceOffData->MoveMode == EFaceOffMoveMode::AttackerToTarget;
	bMovementInterrupted = false;

	if (!bSourceMovementFinished)
	{
		FMuksiBattleMovementFinished SourceFinished;
		SourceFinished.BindUObject(this, &UFaceOffExecution::HandleSourceMovementFinished);
		SourceMovementComponent->StartLinearMove(SourceStageLocation, FaceOffData->MoveDuration, SourceFinished);
	}

	if (!bTargetMovementFinished)
	{
		FMuksiBattleMovementFinished TargetFinished;
		TargetFinished.BindUObject(this, &UFaceOffExecution::HandleTargetMovementFinished);
		TargetMovementComponent->StartLinearMove(TargetStageLocation, FaceOffData->MoveDuration, TargetFinished);
	}
}

ABattleCharacterBase* UFaceOffExecution::ResolveTargetCharacter(const FBattleExecutionContext& Context, EBattleExecutionTargetPolicy TargetPolicy) const
{
	switch (TargetPolicy)
	{
	case EBattleExecutionTargetPolicy::ExecutionTarget:
		return Context.ExecutionTarget;

	case EBattleExecutionTargetPolicy::Attacker:
		return Context.Attacker;

	case EBattleExecutionTargetPolicy::TargetingResult:
	default:
		break;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!StepResult)
		return nullptr;

	for (ABattleCharacterBase* Target : StepResult->GetAllTargets())
	{
		if (IsValid(Target) && Target != Context.Attacker)
			return Target;
	}

	return nullptr;
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
