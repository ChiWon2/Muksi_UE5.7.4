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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.01"))
	float PlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	FName FXDataAssetKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	bool bWaitForFX = false;
};
