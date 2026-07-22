#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/BattleExecution.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "BattleExecutionChain.generated.h"

UCLASS()
class MUKSI_API UBattleExecutionChain : public UBattleExecution
{
	GENERATED_BODY()

public:
	void InitializeChain(const TArray<FBattleExecutionEntry>& InExecutionEntries);
	virtual void Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished) override;

private:
	void ExecuteNextExecution();
	void HandleCurrentExecutionFinished(UBattleExecution* FinishedExecution);
	void FinishChain();

private:
	UPROPERTY(Transient)
	TArray<FBattleExecutionEntry> ExecutionEntries;

	UPROPERTY(Transient)
	TObjectPtr<UBattleExecution> CurrentExecution = nullptr;

	FBattleExecutionContext CachedContext;
	FBattleExecutionFinished CachedOnFinished;

	int32 CurrentExecutionIndex = INDEX_NONE;
	bool bWaitingForCurrentExecution = false;
	bool bChainFinished = false;
};