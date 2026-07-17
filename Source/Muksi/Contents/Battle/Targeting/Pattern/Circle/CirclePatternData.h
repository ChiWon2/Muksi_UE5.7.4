#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "CirclePatternData.generated.h"

USTRUCT(BlueprintType)
struct FCirclePatternData : public FAreaPatternData
{
	GENERATED_BODY()

	// 선택한 중심 좌표로부터 실제 효과가 적용되는 최대 Hex 거리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Circle Pattern", meta = (ClampMin = "0"))
	int32 Radius = 1;

	// 선택한 중심 좌표를 실제 효과 범위에 포함할지 결정한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Circle Pattern")
	bool bIncludeCenterCoord = true;
};