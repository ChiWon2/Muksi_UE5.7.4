#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecutionChain.h"

void UMuksiBattleExecutionChain::InitializeChain(const TArray<FMuksiBattleExecutionEntry>& InExecutionEntries)
{
	ExecutionEntries = InExecutionEntries;
}

void UMuksiBattleExecutionChain::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	CachedContext = Context;
	CachedOnFinished = OnFinished;
	CurrentExecutionIndex = INDEX_NONE;
	bChainFinished = false;

	ExecuteNextExecution();
}

void UMuksiBattleExecutionChain::ExecuteNextExecution()
{
	if (bChainFinished)
	{
		return;
	}

	++CurrentExecutionIndex;

	while (ExecutionEntries.IsValidIndex(CurrentExecutionIndex) && !ExecutionEntries[CurrentExecutionIndex].IsValid())
	{
		++CurrentExecutionIndex;
	}

	if (!ExecutionEntries.IsValidIndex(CurrentExecutionIndex))
	{
		FinishChain();
		return;
	}

	const FMuksiBattleExecutionEntry& Entry = ExecutionEntries[CurrentExecutionIndex];

	if (Entry.ExecutionClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ExecuteNextExecution();
		return;
	}

	CurrentExecution = NewObject<UMuksiBattleExecution>(this, Entry.ExecutionClass);

	if (!CurrentExecution)
	{
		ExecuteNextExecution();
		return;
	}

	FMuksiBattleExecutionContext ExecutionContext = CachedContext;
	ExecutionContext.ExecutionData = Entry.ExecutionData;

	FMuksiBattleExecutionFinished OnExecutionFinished;
	OnExecutionFinished.BindUObject(this, &UMuksiBattleExecutionChain::HandleCurrentExecutionFinished);

	CurrentExecution->Execute(ExecutionContext, OnExecutionFinished);
}

void UMuksiBattleExecutionChain::HandleCurrentExecutionFinished()
{
	CurrentExecution = nullptr;
	ExecuteNextExecution();
}

void UMuksiBattleExecutionChain::FinishChain()
{
	if (bChainFinished)
	{
		return;
	}

	bChainFinished = true;
	CurrentExecution = nullptr;
	ExecutionEntries.Empty();

	FinishExecution(CachedOnFinished);
}
