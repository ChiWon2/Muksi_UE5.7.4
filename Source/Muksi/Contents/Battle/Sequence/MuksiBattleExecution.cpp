#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"

void UMuksiBattleExecution::Execute(const FMuksiBattleExecutionContext& Context,FMuksiBattleExecutionFinished OnFinished)
{
	FinishExecution(OnFinished);
}

void UMuksiBattleExecution::FinishExecution(FMuksiBattleExecutionFinished& OnFinished) const
{
	OnFinished.ExecuteIfBound();
}