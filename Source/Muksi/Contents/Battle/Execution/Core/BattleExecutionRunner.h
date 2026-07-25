#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecution.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleExecutionRunner.generated.h"

class UBattleExecutionRunner;

DECLARE_DELEGATE_OneParam(FBattleExecutionRunnerFinished, UBattleExecutionRunner*);

UCLASS()
class MUKSI_API UBattleExecutionRunner : public UObject
{
	GENERATED_BODY()

public:
	void Run(const TArray<FBattleExecutionEntry>& InExecutionEntries, const FBattleExecutionContext& Context, FBattleExecutionRunnerFinished OnFinished);

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
	FBattleExecutionRunnerFinished CachedOnFinished;

	int32 CurrentExecutionIndex = INDEX_NONE;
	bool bWaitingForCurrentExecution = false;
	bool bRunnerFinished = false;
};
