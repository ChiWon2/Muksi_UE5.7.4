#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecutionChain.h"

void UMuksiBattleExecutionChain::InitializeChain(const TArray<FMuksiBattleExecutionEntry>& InExecutionEntries)
{
	ExecutionEntries = InExecutionEntries;
	CurrentExecution = nullptr;
	CurrentExecutionIndex = INDEX_NONE;
	bWaitingForCurrentExecution = false;
	bChainFinished = false;
}

void UMuksiBattleExecutionChain::Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished)
{
	CachedContext = Context;
	CachedOnFinished = OnFinished;
	CurrentExecution = nullptr;
	CurrentExecutionIndex = INDEX_NONE;
	bWaitingForCurrentExecution = false;
	bChainFinished = false;

	ExecuteNextExecution();
}

void UMuksiBattleExecutionChain::ExecuteNextExecution()
{
	if (bChainFinished || bWaitingForCurrentExecution)
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

	if (!Entry.ExecutionClass || Entry.ExecutionClass->HasAnyClassFlags(CLASS_Abstract))
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

	UMuksiBattleExecution* StartedExecution = CurrentExecution.Get();

	FMuksiBattleExecutionFinished OnExecutionFinished;
	OnExecutionFinished.BindUObject(this, &UMuksiBattleExecutionChain::HandleCurrentExecutionFinished, StartedExecution);

	bWaitingForCurrentExecution = true;
	StartedExecution->Execute(ExecutionContext, OnExecutionFinished);
}

void UMuksiBattleExecutionChain::HandleCurrentExecutionFinished(UMuksiBattleExecution* FinishedExecution)
{
	if (bChainFinished || !bWaitingForCurrentExecution || !FinishedExecution || FinishedExecution != CurrentExecution)
	{
		return;
	}

	bWaitingForCurrentExecution = false;
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
	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;
	ExecutionEntries.Empty();
	CachedContext = FMuksiBattleExecutionContext();
	CurrentExecutionIndex = INDEX_NONE;

	FMuksiBattleExecutionFinished OnChainFinished = CachedOnFinished;
	CachedOnFinished.Unbind();

	FinishExecution(OnChainFinished);
}