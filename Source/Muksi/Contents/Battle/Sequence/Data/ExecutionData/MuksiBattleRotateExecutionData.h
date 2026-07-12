#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Sequence/Data/MuksiBattleSequenceDataTypes.h"
#include "MuksiBattleRotateExecutionData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiBattleRotateExecutionData : public FMuksiBattleExecutionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rotation", meta = (ClampMin = "0.0"))
	float RotationSpeed = 720.0f;
};