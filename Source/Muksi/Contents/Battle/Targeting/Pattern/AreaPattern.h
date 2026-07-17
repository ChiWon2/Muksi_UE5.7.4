#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPatternContext.h"

#include "AreaPattern.generated.h"

UCLASS(Abstract)
class MUKSI_API UAreaPattern : public UObject
{
	GENERATED_BODY()

public:
	// 확정된 Targeting Step 정보를 사용해 실제 효과 좌표와 경로 좌표를 계산한다.
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const;

	// 현재 Pattern이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetPatternDataStruct() const;

protected:
	// 전달된 PatternData가 현재 Pattern이 요구하는 구조체 타입인지 확인한다.
	bool IsPatternDataValid(const FInstancedStruct& PatternData) const;

	// 실제 효과 범위 좌표를 중복 없이 추가한다.
	void AddAffectedCoord(FTargetingResult& InOutResult, const FIntPoint& Coord) const;

	// 이동 또는 투사체 경로 좌표를 중복 없이 추가한다.
	void AddPathCoord(FTargetingResult& InOutResult, const FIntPoint& Coord) const;
};

#define AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData) \
	do \
	{ \
		if (!(Context).GridManager) \
		{ \
			return; \
		} \
		if (!IsPatternDataValid(PatternData)) \
		{ \
			return; \
		} \
	} \
	while (false)

