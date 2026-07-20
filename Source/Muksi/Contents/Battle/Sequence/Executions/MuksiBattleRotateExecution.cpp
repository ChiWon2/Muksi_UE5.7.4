#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleRotateExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Sequence/Data/ExecutionData/MuksiBattleRotateExecutionData.h"

void UMuksiBattleRotateExecution::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	RotatingCharacter = Context.Attacker.Get();

	if (!RotatingCharacter || !Context.BattleGridManager)
	{
		FinishRotateExecution();
		return;
	}

	const FMuksiBattleRotateExecutionData* RotateData = Context.GetExecutionData<FMuksiBattleRotateExecutionData>();

	if (!RotateData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[RotateExecution] RotateExecutionData is invalid."));
		FinishRotateExecution();
		return;
	}

	MovementComponent = RotatingCharacter->GetBattleMovementComponent();

	if (!MovementComponent)
	{
		FinishRotateExecution();
		return;
	}

	const FIntPoint TargetPoint = Context.GetMainTargetPoint();

	if (TargetPoint.X == INDEX_NONE || TargetPoint.Y == INDEX_NONE)
	{
		FinishRotateExecution();
		return;
	}

	const FVector TargetWorldLocation = Context.BattleGridManager->GetTransformToPosition(TargetPoint).GetLocation();

	FMuksiBattleMovementFinished OnRotationFinished;
	OnRotationFinished.BindUObject(this, &UMuksiBattleRotateExecution::HandleRotationFinished);

	MovementComponent->StartRotateTowardLocation(TargetWorldLocation, RotateData->RotationSpeed, OnRotationFinished);
}

void UMuksiBattleRotateExecution::HandleRotationFinished(bool bInterrupted)
{
	FinishRotateExecution();
}

void UMuksiBattleRotateExecution::FinishRotateExecution()
{
	if (IsExecutionFinished())
	{
		return;
	}

	RotatingCharacter = nullptr;
	MovementComponent = nullptr;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UMuksiBattleRotateExecution::GetExecutionDataStruct() const
{
	return FMuksiBattleRotateExecutionData::StaticStruct();
}