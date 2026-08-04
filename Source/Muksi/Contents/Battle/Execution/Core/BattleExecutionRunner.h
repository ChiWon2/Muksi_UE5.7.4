#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleExecutionRunner.generated.h"

class UBattleExecutionRunner;

DECLARE_DELEGATE_OneParam(FBattleExecutionRunnerFinished, UBattleExecutionRunner*);
DECLARE_DELEGATE_ThreeParams(FBattleExecutionEntryStarted, const FBattleExecutionEntry&, int32, FBattleExecutionContext&);

UCLASS()
class MUKSI_API UBattleExecutionRunner : public UObject
{
	GENERATED_BODY()

public:
	void Run(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionEntryStarted OnEntryStarted, FBattleExecutionRunnerFinished OnFinished);

private:
	void ExecuteNextExecution();
	void HandleCurrentExecutionFinished();
	void FinishRunner();
	bool ShouldExecuteEntry(const FBattleExecutionEntry& Entry) const;

private:
	UPROPERTY(Transient)
	TArray<FBattleExecutionEntry> ExecutionEntries;

	UPROPERTY(Transient)
	TObjectPtr<UBattleExecution> CurrentExecution = nullptr;

	FBattleExecutionContext CachedContext;
	FBattleExecutionEntryStarted CachedOnEntryStarted;
	FBattleExecutionRunnerFinished CachedOnFinished;

	int32 CurrentExecutionIndex = INDEX_NONE;
	bool bWaitingForCurrentExecution = false;
	bool bRunnerFinished = false;
};
