#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "PointPattern.generated.h"

UCLASS()
class MUKSI_API UPointPattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	// 마지막 Step에서 선택한 대표 좌표 하나를 실제 효과 범위에 추가한다.
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;

	// PointPattern이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetPatternDataStruct() const override;
};