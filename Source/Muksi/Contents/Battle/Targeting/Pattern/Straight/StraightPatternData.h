#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "StraightPatternData.generated.h"

USTRUCT(BlueprintType)
struct FStraightPatternData : public FAreaPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "1"))
	int32 Range = 3;
};