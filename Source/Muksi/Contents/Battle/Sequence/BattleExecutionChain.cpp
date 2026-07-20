#include "Muksi/Contents/Battle/Sequence/BattleExecutionChain.h"

void UBattleExecutionChain::InitializeChain(const TArray<FBattleExecutionEntry>& InExecutionEntries)
{
	ExecutionEntries = InExecutionEntries;
	CurrentExecution = nullptr;
	CurrentExecutionIndex = INDEX_NONE;
	bWaitingForCurrentExecution = false;
	bChainFinished = false;
}

void UBattleExecutionChain::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedContext = Context;
	CachedOnFinished = OnFinished;
	CurrentExecution = nullptr;
	CurrentExecutionIndex = INDEX_NONE;
	bWaitingForCurrentExecution = false;
	bChainFinished = false;

	ExecuteNextExecution();
}

void UBattleExecutionChain::ExecuteNextExecution()
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

	const FBattleExecutionEntry& Entry = ExecutionEntries[CurrentExecutionIndex];

	if (!Entry.ExecutionClass || Entry.ExecutionClass->HasAnyClassFlags(CLASS_Abstract))
	{
		ExecuteNextExecution();
		return;
	}

	CurrentExecution = NewObject<UBattleExecution>(this, Entry.ExecutionClass);

	if (!CurrentExecution)
	{
		ExecuteNextExecution();
		return;
	}

	FBattleExecutionContext ExecutionContext = CachedContext;
	ExecutionContext.ExecutionData = Entry.ExecutionData;

	UBattleExecution* StartedExecution = CurrentExecution.Get();

	FBattleExecutionFinished OnExecutionFinished;
	OnExecutionFinished.BindUObject(this, &UBattleExecutionChain::HandleCurrentExecutionFinished, StartedExecution);

	bWaitingForCurrentExecution = true;
	StartedExecution->Execute(ExecutionContext, OnExecutionFinished);
}

void UBattleExecutionChain::HandleCurrentExecutionFinished(UBattleExecution* FinishedExecution)
{
	if (bChainFinished || !bWaitingForCurrentExecution || !FinishedExecution || FinishedExecution != CurrentExecution)
	{
		return;
	}

	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;

	ExecuteNextExecution();
}

void UBattleExecutionChain::FinishChain()
{
	if (bChainFinished)
	{
		return;
	}

	bChainFinished = true;
	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;
	ExecutionEntries.Empty();
	CachedContext = FBattleExecutionContext();
	CurrentExecutionIndex = INDEX_NONE;

	FBattleExecutionFinished OnChainFinished = CachedOnFinished;
	CachedOnFinished.Unbind();

	FinishExecution(OnChainFinished);
}