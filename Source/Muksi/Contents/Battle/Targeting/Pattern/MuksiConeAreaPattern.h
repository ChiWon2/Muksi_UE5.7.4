#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"
#include "MuksiConeAreaPattern.generated.h"

struct FCubeCoord;

UCLASS()
class MUKSI_API UMuksiConeAreaPattern : public UMuksiCardAreaPattern
{
	GENERATED_BODY()

public:
	// 시전자에서 조준 위치를 향하는 원뿔형 범위에 포함된 좌표를 실제 효과 범위에 추가한다.
	virtual void ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const override;

	// Cone Area Pattern이 사용하는 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetAreaPatternDataStruct() const override;

private:
	// 두 Cube 좌표 사이의 Hex 거리를 반환한다.
	int32 GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B) const;

	// 시전자에서 후보 타일까지의 평면 방향이 Cone 범위 안에 있는지 확인한다.
	bool IsDirectionInsideCone(const FVector& SourceWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const;
};