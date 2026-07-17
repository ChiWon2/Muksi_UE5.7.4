#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternData.h"

#include "ConePatternData.generated.h"

USTRUCT(BlueprintType)
struct FConePatternData : public FAreaPatternData
{
	GENERATED_BODY()

	// Origin으로부터 실제 효과가 적용되는 최대 Hex 거리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern", meta = (ClampMin = "1"))
	int32 Range = 1;

	// 조준 방향을 중심으로 사용하는 전체 Cone 각도.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern", meta = (ClampMin = "1.0", ClampMax = "360.0"))
	float Angle = 60.0f;

	// Origin 좌표를 실제 효과 범위에 포함할지 결정한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cone Pattern")
	bool bIncludeOriginCoord = false;
};