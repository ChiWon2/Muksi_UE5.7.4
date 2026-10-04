#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "StopMontageExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FStopMontageExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation", meta = (ClampMin = "0.0"))
	float BlendOutTime = 0.05f;
};
