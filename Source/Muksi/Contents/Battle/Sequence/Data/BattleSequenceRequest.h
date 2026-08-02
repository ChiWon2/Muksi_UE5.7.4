#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleSequenceRequest.generated.h"

class UMuksiBattleCardDataAsset;

USTRUCT(BlueprintType)
struct FBattleSequenceRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Battle|Sequence")
	FBattleAction Action;

	UPROPERTY(BlueprintReadWrite, Category = "Battle|Sequence")
	EBattleExecutionMode ExecutionMode = EBattleExecutionMode::Sequence;

	UPROPERTY(BlueprintReadWrite, Category = "Battle|Sequence")
	TObjectPtr<UMuksiBattleCardDataAsset> ExecutionCardOverride = nullptr;

	UMuksiBattleCardDataAsset* GetExecutionCard() const
	{
		return ExecutionCardOverride ? ExecutionCardOverride.Get() : Action.Card.Get();
	}
};
