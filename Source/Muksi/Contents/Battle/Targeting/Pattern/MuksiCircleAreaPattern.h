#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"
#include "MuksiCircleAreaPattern.generated.h"

struct FCubeCoord;

UCLASS()
class MUKSI_API UMuksiCircleAreaPattern : public UMuksiCardAreaPattern
{
	GENERATED_BODY()

public:
	// 선택된 대표 좌표를 중심으로 Hex 반경 내 좌표를 실제 효과 범위에 추가한다.
	virtual void ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const override;

	// Circle Area Pattern이 사용하는 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetAreaPatternDataStruct() const override;

private:
	// 두 Cube 좌표 사이의 Hex 거리를 반환한다.
	int32 GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B) const;
};