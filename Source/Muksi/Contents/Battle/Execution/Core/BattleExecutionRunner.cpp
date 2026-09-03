#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"


void UBattleExecutionRunner::RunExecutions(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionEntryStarted OnEntryStarted, FBattleExecutionEntryFinished OnEntryFinished, FBattleExecutionRunnerFinished OnFinished)
{
	ExecutionEntries = InExecutionEntries;
	CachedContext = Context;
	CachedContext.RequestRuntimeExecutionChain.BindUObject(this, &UBattleExecutionRunner::HandleRuntimeExecutionChainRequested);
	CurrentExecutionContext = FBattleExecutionContext();
	CachedOnEntryStarted = OnEntryStarted;
	CachedOnEntryFinished = OnEntryFinished;
	CachedOnFinished = OnFinished;
	CurrentExecution = nullptr;
	ActiveNestedExecutionRunners.Reset();
	NestedRunnerCompletionDelegates.Reset();
	CurrentExecutionIndex = INDEX_NONE;
	bWaitingForCurrentExecution = false;
	bRunnerFinished = false;

	ExecuteNextExecution();
}

void UBattleExecutionRunner::ExecuteNextExecution()
{
	if (bRunnerFinished || bWaitingForCurrentExecution)
	{
		return;
	}

	++CurrentExecutionIndex;

	while (ExecutionEntries.IsValidIndex(CurrentExecutionIndex) && !ShouldExecuteEntry(ExecutionEntries[CurrentExecutionIndex]))
	{
		++CurrentExecutionIndex;
	}

	if (!ExecutionEntries.IsValidIndex(CurrentExecutionIndex))
	{
		TryCompleteExecutionSequence();
		return;
	}

	const FBattleExecutionEntry& Entry = ExecutionEntries[CurrentExecutionIndex];

	CurrentExecution = NewObject<UBattleExecution>(this, Entry.ExecutionClass);

	if (!CurrentExecution)
	{
		ExecuteNextExecution();
		return;
	}

	FBattleExecutionContext ExecutionContext = CachedContext;
	ExecutionContext.ExecutionData = Entry.ExecutionData;

	FBattleExecutionFinished OnExecutionFinished;
	OnExecutionFinished.BindUObject(this, &UBattleExecutionRunner::HandleCurrentExecutionFinished);

	bWaitingForCurrentExecution = true;
	CachedOnEntryStarted.ExecuteIfBound(Entry, CurrentExecutionIndex, ExecutionContext);
	CurrentExecutionContext = ExecutionContext;
	CurrentExecution->Execute(CurrentExecutionContext, OnExecutionFinished);
}

void UBattleExecutionRunner::HandleCurrentExecutionFinished()
{
	if (bRunnerFinished || !bWaitingForCurrentExecution)
	{
		return;
	}

	const int32 FinishedEntryIndex = CurrentExecutionIndex;
	const FBattleExecutionEntry FinishedEntry = ExecutionEntries.IsValidIndex(FinishedEntryIndex)
		? ExecutionEntries[FinishedEntryIndex]
		: FBattleExecutionEntry();
	const FBattleExecutionContext FinishedContext = CurrentExecutionContext;

	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;
	CurrentExecutionContext = FBattleExecutionContext();

	if (FinishedEntry.IsValid())
	{
		CachedOnEntryFinished.ExecuteIfBound(FinishedEntry, FinishedEntryIndex, FinishedContext);
	}

	ExecuteNextExecution();
}

bool UBattleExecutionRunner::HandleRuntimeExecutionChainRequested(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FSimpleDelegate CompletionDelegate)
{
	if (bRunnerFinished || !bWaitingForCurrentExecution || InExecutionEntries.IsEmpty())
	{
		return false;
	}

	if (Context.NestedChainDepth >= MaxNestedChainDepth)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleExecutionRunner] Max nested Chain depth reached. Depth=%d"), Context.NestedChainDepth);
		return false;
	}

	UBattleExecutionRunner* NestedRunner = NewObject<UBattleExecutionRunner>(this);
	if (!NestedRunner)
	{
		return false;
	}

	FBattleExecutionContext NestedContext = Context;
	NestedContext.NestedChainDepth = Context.NestedChainDepth + 1;

	ActiveNestedExecutionRunners.Add(NestedRunner);
	NestedRunnerCompletionDelegates.Add(NestedRunner, MoveTemp(CompletionDelegate));

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UBattleExecutionRunner::HandleNestedExecutionRunnerFinished);
	NestedRunner->RunExecutions(InExecutionEntries, NestedContext, CachedOnEntryStarted, CachedOnEntryFinished, OnFinished);
	return true;
}

void UBattleExecutionRunner::HandleNestedExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (bRunnerFinished || !FinishedRunner || ActiveNestedExecutionRunners.RemoveSingle(FinishedRunner) == 0)
	{
		return;
	}

	FSimpleDelegate CompletionDelegate;
	if (FSimpleDelegate* FoundDelegate = NestedRunnerCompletionDelegates.Find(FinishedRunner))
	{
		CompletionDelegate = MoveTemp(*FoundDelegate);
		NestedRunnerCompletionDelegates.Remove(FinishedRunner);
	}

	CompletionDelegate.ExecuteIfBound();
	TryCompleteExecutionSequence();
}

void UBattleExecutionRunner::TryCompleteExecutionSequence()
{
	if (!bRunnerFinished && !bWaitingForCurrentExecution && ActiveNestedExecutionRunners.IsEmpty() && !ExecutionEntries.IsValidIndex(CurrentExecutionIndex))
	{
		CompleteExecutionSequence();
	}
}

void UBattleExecutionRunner::CompleteExecutionSequence()
{
	if (bRunnerFinished)
	{
		return;
	}

	bRunnerFinished = true;
	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;
	ExecutionEntries.Empty();
	ActiveNestedExecutionRunners.Empty();
	NestedRunnerCompletionDelegates.Empty();
	CachedContext = FBattleExecutionContext();
	CurrentExecutionContext = FBattleExecutionContext();
	CurrentExecutionIndex = INDEX_NONE;

	FBattleExecutionRunnerFinished OnRunnerFinished = CachedOnFinished;
	CachedOnEntryStarted.Unbind();
	CachedOnEntryFinished.Unbind();
	CachedOnFinished.Unbind();
	OnRunnerFinished.ExecuteIfBound(this);
}

bool UBattleExecutionRunner::ShouldExecuteEntry(const FBattleExecutionEntry& Entry) const
{
	if (!Entry.IsValid() || !Entry.ExecutionClass || Entry.ExecutionClass->HasAnyClassFlags(CLASS_Abstract))
	{
		return false;
	}

	if (Entry.ExecutionScope == EBattleExecutionScope::Both)
	{
		return true;
	}

	if (CachedContext.ExecutionMode == EBattleExecutionMode::Simulation)
	{
		return Entry.ExecutionScope == EBattleExecutionScope::SimulationOnly;
	}

	return Entry.ExecutionScope == EBattleExecutionScope::SequenceOnly;
}
