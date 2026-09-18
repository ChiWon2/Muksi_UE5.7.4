#include "Muksi/Contents/Battle/Execution/Executions/Rotate/RotateExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Core/BattleGridCell.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Execution/Executions/Rotate/RotateExecutionData.h"

void URotateExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	RotatingCharacter = Context.Attacker.Get();

	if (!IsValid(RotatingCharacter) || !IsValid(Context.BattleGridManager))
	{
		FinishRotateExecution();
		return;
	}

	const FRotateExecutionData* RotateData = Context.GetExecutionData<FRotateExecutionData>();

	if (!RotateData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RotateExecution] RotateExecutionData is invalid."));
		FinishRotateExecution();
		return;
	}

	MovementComponent = RotatingCharacter->GetBattleMovementComponent();

	if (!IsValid(MovementComponent))
	{
		FinishRotateExecution();
		return;
	}

	if (RotateData->TargetMode == ERotateExecutionTargetMode::Opponent)
	{
		OpponentCharacter = FindOpponentCharacter(Context);

		if (!IsValid(OpponentCharacter))
		{
			FinishRotateExecution();
			return;
		}

		FMuksiBattleMovementFinished OnRotationFinished;
		OnRotationFinished.BindUObject(this, &URotateExecution::HandleRotationFinished);

		MovementComponent->StartRotateTowardLocation(
			OpponentCharacter->GetActorLocation(),
			RotateData->RotationSpeed,
			OnRotationFinished);
		return;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!StepResult || !StepResult->Step.HasTargetCoord())
	{
		FinishRotateExecution();
		return;
	}

	const FVector TargetWorldLocation = Context.BattleGridManager
		->GetTransformToPosition(StepResult->Step.TargetCoord)
		.GetLocation();

	FMuksiBattleMovementFinished OnRotationFinished;
	OnRotationFinished.BindUObject(this, &URotateExecution::HandleRotationFinished);

	MovementComponent->StartRotateTowardLocation(
		TargetWorldLocation,
		RotateData->RotationSpeed,
		OnRotationFinished);
}

ABattleCharacterBase* URotateExecution::FindOpponentCharacter(const FBattleExecutionContext& Context) const
{
	if (!IsValid(Context.BattleGridManager) || !IsValid(RotatingCharacter))
		return nullptr;

	for (const FBattleGridCell& Cell : Context.BattleGridManager->GetGridCells(Context.GridWorldType))
	{
		ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(Cell.OccupyingActor.Get());

		if (IsValid(Character) && Character != RotatingCharacter)
			return Character;
	}

	return nullptr;
}

void URotateExecution::HandleRotationFinished(bool bInterrupted)
{
	FinishRotateExecution();
}

void URotateExecution::FinishRotateExecution()
{
	if (IsExecutionFinished())
		return;

	RotatingCharacter = nullptr;
	OpponentCharacter = nullptr;
	MovementComponent = nullptr;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* URotateExecution::GetExecutionDataStruct() const
{
	return FRotateExecutionData::StaticStruct();
}
