#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Data/MuksiCardAreaPatternData.h"
#include "MuksiConeAreaPatternData.generated.h"

USTRUCT(BlueprintType)
struct FMuksiConeAreaPatternData : public FMuksiCardAreaPatternData
{
	GENERATED_BODY()

	// 시전자 위치에서 효과가 도달할 최대 Hex 거리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Pattern", meta = (ClampMin = "1"))
	int32 Range = 3;

	// 원뿔형 효과 범위의 전체 각도.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Pattern", meta = (ClampMin = "1.0", ClampMax = "360.0"))
	float Angle = 60.0f;

	// 시전자가 위치한 시작 타일을 효과 범위에 포함할지 결정한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area Pattern")
	bool bIncludeSourceCoord = false;
};