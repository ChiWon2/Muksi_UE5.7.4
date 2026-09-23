#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"
#include "MultiDirectionLinePatternData.generated.h"

USTRUCT(BlueprintType)
struct FMultiDirectionLinePatternData : public FAreaPatternData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "1"))
	int32 Range = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern")
	TArray<int32> DirectionOffsets = { -1, 0, 1 };
};
