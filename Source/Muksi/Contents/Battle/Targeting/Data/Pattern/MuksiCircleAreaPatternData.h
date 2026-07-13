#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Data/MuksiCardAreaPatternData.h"
#include "MuksiCircleAreaPatternData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiCircleAreaPatternData : public FMuksiCardAreaPatternData
{
	GENERATED_BODY()

	// 선택된 대표 좌표를 중심으로 효과를 적용할 Hex 타일 반경.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Pattern", meta = (ClampMin = "0"))
	int32 Radius = 1;
};