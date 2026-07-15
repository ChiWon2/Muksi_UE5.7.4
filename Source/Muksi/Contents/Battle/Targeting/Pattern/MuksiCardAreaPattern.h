#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingResult.h"

#include "MuksiCardAreaPattern.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiCardAreaPattern : public UObject
{
	GENERATED_BODY()

public:
	// 선택 결과를 기준으로 실제 적용 범위와 경로를 계산한다.
	virtual void ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const;

	// 이 Pattern이 사용하는 AreaPatternData 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetAreaPatternDataStruct() const;

protected:
	// 전달된 PatternData가 이 Pattern에서 요구하는 타입인지 확인한다.
	bool IsAreaPatternDataValid(const FInstancedStruct& PatternData) const;

	// 계산된 좌표를 중복 없이 결과의 효과 범위에 추가한다.
	void AddAffectedCoord(FMuksiCardTargetingResult& InOutResult, const FIntPoint& Coord) const;

	// 계산된 좌표를 중복 없이 결과의 경로에 추가한다.
	void AddPathCoord(FMuksiCardTargetingResult& InOutResult, const FIntPoint& Coord) const;
};