#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "MuksiBattleExecutionChain.generated.h"

UCLASS()
class MUKSI_API UMuksiBattleExecutionChain : public UMuksiBattleExecution
{
	GENERATED_BODY()

public:
	void InitializeChain(const TArray<FMuksiBattleExecutionEntry>& InExecutionEntries);
	virtual void Execute(const FMuksiBattleExecutionContext& Context, FMuksiBattleExecutionFinished OnFinished) override;

private:
	void ExecuteNextExecution();
	void HandleCurrentExecutionFinished();
	void FinishChain();

private:
	UPROPERTY(Transient)
	TArray<FMuksiBattleExecutionEntry> ExecutionEntries;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleExecution> CurrentExecution = nullptr;

	FMuksiBattleExecutionContext CachedContext;
	FMuksiBattleExecutionFinished CachedOnFinished;

	int32 CurrentExecutionIndex = INDEX_NONE;
	bool bChainFinished = false;
};