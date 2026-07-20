#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"

void UBattleExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	FinishExecution(OnFinished);
}

const UScriptStruct* UBattleExecution::GetExecutionDataStruct() const
{
	return nullptr;
}

void UBattleExecution::FinishExecution(FBattleExecutionFinished& OnFinished)
{
	if (bExecutionFinished)
	{
		return;
	}

	bExecutionFinished = true;

	FBattleExecutionFinished FinishedDelegate = OnFinished;
	OnFinished.Unbind();
	FinishedDelegate.ExecuteIfBound();
}