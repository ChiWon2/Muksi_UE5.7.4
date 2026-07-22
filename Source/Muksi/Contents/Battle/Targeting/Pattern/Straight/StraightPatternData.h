#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "StraightPatternData.generated.h"

USTRUCT(BlueprintType)
struct FStraightPatternData : public FAreaPatternData
{
	GENERATED_BODY()

	// 원점에서 직선으로 탐색할 최대 Grid 거리.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pattern", meta = (ClampMin = "1"))
	int32 Range = 5;
};