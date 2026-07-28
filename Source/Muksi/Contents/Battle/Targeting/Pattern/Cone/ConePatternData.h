#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "ConePatternData.generated.h"

USTRUCT(BlueprintType)
struct FConePatternData : public FAreaPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern", meta = (ClampMin = "1"))
	int32 Range = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern", meta = (ClampMin = "1.0", ClampMax = "360.0"))
	float Angle = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern")
	bool bIncludeOriginCoord = false;
};