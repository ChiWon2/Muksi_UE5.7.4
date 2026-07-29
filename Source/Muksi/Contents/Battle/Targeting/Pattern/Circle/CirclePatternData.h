#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "CirclePatternData.generated.h"

USTRUCT(BlueprintType)
struct FCirclePatternData : public FAreaPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Circle Pattern", meta = (ClampMin = "0"))
	int32 Radius = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Circle Pattern")
	bool bIncludeCenterCoord = true;
};