#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "RotateExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FRotateExecutionData : public FBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation", meta = (ClampMin = "0.0"))
	float RotationSpeed = 720.0f;
};