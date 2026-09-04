#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleExecutionRunner.generated.h"

class UBattleExecutionRunner;

DECLARE_DELEGATE_OneParam(FBattleExecutionRunnerFinished, UBattleExecutionRunner*);
DECLARE_DELEGATE_ThreeParams(FBattleExecutionEntryStarted, const FBattleExecutionEntry&, int32, FBattleExecutionContext&);
DECLARE_DELEGATE_ThreeParams(FBattleExecutionEntryFinished, const FBattleExecutionEntry&, int32, const FBattleExecutionContext&);

UCLASS()
class MUKSI_API UBattleExecutionRunner : public UObject
{
	GENERATED_BODY()

public:
	void RunExecutionEntries(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionEntryStarted OnEntryStarted, FBattleExecutionEntryFinished OnEntryFinished, FBattleExecutionRunnerFinished OnFinished);

private:
	void ExecuteNextExecution();
	void HandleCurrentExecutionFinished();
	bool HandleRuntimeExecutionEntriesRequested(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context, FSimpleDelegate CompletionDelegate);
	void HandleNestedExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
	void TryCompleteRunner();
	void CompleteRunner();
	bool ShouldExecuteEntry(const FBattleExecutionEntry& Entry) const;

private:
	UPROPERTY(Transient)
	TArray<FBattleExecutionEntry> ExecutionEntries;

	UPROPERTY(Transient)
	TObjectPtr<UBattleExecution> CurrentExecution = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBattleExecutionRunner>> ActiveNestedExecutionRunners;

	FBattleExecutionContext CachedContext;
	FBattleExecutionContext CurrentExecutionContext;
	TMap<UBattleExecutionRunner*, FSimpleDelegate> NestedRunnerCompletionDelegates;
	FBattleExecutionEntryStarted CachedOnEntryStarted;
	FBattleExecutionEntryFinished CachedOnEntryFinished;
	FBattleExecutionRunnerFinished CachedOnFinished;

	int32 CurrentExecutionIndex = INDEX_NONE;
	bool bWaitingForCurrentExecution = false;
	bool bRunnerFinished = false;
	static constexpr int32 MaxNestedRunnerDepth = 16;
};
