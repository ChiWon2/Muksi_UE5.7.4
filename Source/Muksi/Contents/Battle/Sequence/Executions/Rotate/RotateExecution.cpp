#include "Muksi/Contents/Battle/Sequence/Executions/Rotate/RotateExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Movement/MuksiBattleMovementComponent.h"
#include "Muksi/Contents/Battle/Sequence/Executions/Rotate/RotateExecutionData.h"

void URotateExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	RotatingCharacter = Context.Attacker.Get();

	if (!RotatingCharacter || !Context.BattleGridManager)
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
	OnRotationFinished.BindUObject(this, &URotateExecution::HandleRotationFinished);

	MovementComponent->StartRotateTowardLocation(TargetWorldLocation, RotateData->RotationSpeed, OnRotationFinished);
}

void URotateExecution::HandleRotationFinished(bool bInterrupted)
{
	FinishRotateExecution();
}

void URotateExecution::FinishRotateExecution()
{
	if (IsExecutionFinished())
	{
		return;
	}

	RotatingCharacter = nullptr;
	MovementComponent = nullptr;

	FinishExecution(CachedOnFinished);
}

const UScriptStruct* URotateExecution::GetExecutionDataStruct() const
{
	return FRotateExecutionData::StaticStruct();
}