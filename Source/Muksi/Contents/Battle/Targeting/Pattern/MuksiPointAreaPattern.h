#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"
#include "MuksiPointAreaPattern.generated.h"

UCLASS()
class MUKSI_API UMuksiPointAreaPattern : public UMuksiCardAreaPattern
{
	GENERATED_BODY()

public:
	// 선택된 대표 좌표 하나를 실제 효과 범위에 추가한다.
	virtual void ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const override;

	// Point Area Pattern이 사용하는 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetAreaPatternDataStruct() const override;
};