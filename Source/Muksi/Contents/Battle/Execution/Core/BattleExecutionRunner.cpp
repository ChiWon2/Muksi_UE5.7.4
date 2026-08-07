#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"

void UBattleExecutionRunner::Run(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionEntryStarted OnEntryStarted, FBattleExecutionEntryFinished OnEntryFinished, FBattleExecutionRunnerFinished OnFinished)
{
	ExecutionEntries = InExecutionEntries;
	CachedContext = Context;
	CurrentExecutionContext = FBattleExecutionContext();
	CachedOnEntryStarted = OnEntryStarted;
	CachedOnEntryFinished = OnEntryFinished;
	CachedOnFinished = OnFinished;
	CurrentExecution = nullptr;
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
		FinishRunner();
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

void UBattleExecutionRunner::FinishRunner()
{
	if (bRunnerFinished)
	{
		return;
	}

	bRunnerFinished = true;
	bWaitingForCurrentExecution = false;
	CurrentExecution = nullptr;
	ExecutionEntries.Empty();
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
