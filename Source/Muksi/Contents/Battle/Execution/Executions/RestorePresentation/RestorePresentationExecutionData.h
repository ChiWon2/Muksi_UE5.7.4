#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "RestorePresentationExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FRestorePresentationExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (ClampMin = "0.0"))
	float MoveDuration = 0.2f;
};
