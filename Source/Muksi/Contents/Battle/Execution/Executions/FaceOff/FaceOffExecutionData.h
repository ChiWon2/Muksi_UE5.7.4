#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "FaceOffExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FFaceOffExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff", meta = (ClampMin = "0.0"))
	float CharacterDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FaceOff", meta = (ClampMin = "0.0"))
	float MoveDuration = 0.2f;
};
