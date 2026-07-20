#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"

void UMuksiBattleExecution::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	FinishExecution(OnFinished);
}

const UScriptStruct* UMuksiBattleExecution::GetExecutionDataStruct() const
{
	return nullptr;
}

void UMuksiBattleExecution::FinishExecution(FMuksiBattleExecutionFinished& OnFinished)
{
	if (bExecutionFinished)
	{
		return;
	}

	bExecutionFinished = true;

	FMuksiBattleExecutionFinished FinishedDelegate = OnFinished;
	OnFinished.Unbind();
	FinishedDelegate.ExecuteIfBound();
}