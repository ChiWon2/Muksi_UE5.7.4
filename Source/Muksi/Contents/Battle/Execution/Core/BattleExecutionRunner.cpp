#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"


void UBattleExecutionRunner::RunExecutionEntries(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionEntryStarted OnEntryStarted, FBattleExecutionEntryFinished OnEntryFinished, FBattleExecutionRunnerFinished OnFinished)
{
	ExecutionEntries = InExecutionEntries;
	CachedContext = Context;
	CachedContext.RequestRuntimeExecutionEntries.BindUObject(this, &UBattleExecutionRunner::HandleRuntimeExecutionEntriesRequested);
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
		TryCompleteRunner();
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

bool UBattleExecutionRunner::HandleRuntimeExecutionEntriesRequested(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FSimpleDelegate CompletionDelegate)
{
	if (bRunnerFinished || !bWaitingForCurrentExecution || InExecutionEntries.IsEmpty())
	{
		return false;
	}

	if (Context.NestedRunnerDepth >= MaxNestedRunnerDepth)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleExecutionRunner] Max nested BattleExecutionRunner depth reached. Depth=%d"), Context.NestedRunnerDepth);
		return false;
	}

	UBattleExecutionRunner* NestedRunner = NewObject<UBattleExecutionRunner>(this);
	if (!NestedRunner)
	{
		return false;
	}

	FBattleExecutionContext NestedContext = Context;
	NestedContext.NestedRunnerDepth = Context.NestedRunnerDepth + 1;

	ActiveNestedExecutionRunners.Add(NestedRunner);
	NestedRunnerCompletionDelegates.Add(NestedRunner, MoveTemp(CompletionDelegate));

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UBattleExecutionRunner::HandleNestedExecutionRunnerFinished);
	NestedRunner->RunExecutionEntries(InExecutionEntries, NestedContext, CachedOnEntryStarted, CachedOnEntryFinished, OnFinished);
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
	TryCompleteRunner();
}

void UBattleExecutionRunner::TryCompleteRunner()
{
	if (!bRunnerFinished && !bWaitingForCurrentExecution && ActiveNestedExecutionRunners.IsEmpty() && !ExecutionEntries.IsValidIndex(CurrentExecutionIndex))
	{
		CompleteRunner();
	}
}

void UBattleExecutionRunner::CompleteRunner()
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

	return Entry.ExecutionScope == EBattleExecutionScope::ActualBattleOnly;
}
