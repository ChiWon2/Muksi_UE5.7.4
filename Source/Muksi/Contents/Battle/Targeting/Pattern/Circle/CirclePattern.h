#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "CirclePattern.generated.h"

UCLASS()
class MUKSI_API UCirclePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	// 마지막 Step의 선택 좌표를 중심으로 Hex 반경 안의 모든 좌표를 실제 효과 범위에 추가한다.
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;

	// CirclePattern이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetPatternDataStruct() const override;
};