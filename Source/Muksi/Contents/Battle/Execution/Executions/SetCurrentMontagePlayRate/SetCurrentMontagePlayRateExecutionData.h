#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "SetCurrentMontagePlayRateExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FSetCurrentMontagePlayRateExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.0"))
	float PlayRate = 1.0f;
};
