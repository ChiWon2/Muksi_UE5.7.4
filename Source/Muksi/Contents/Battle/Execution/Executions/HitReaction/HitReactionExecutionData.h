#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "HitReactionExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FHitReactionExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	FName AnimKey = TEXT("HitReaction");
};
