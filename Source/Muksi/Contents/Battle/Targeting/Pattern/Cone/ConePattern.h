#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

#include "ConePattern.generated.h"

UCLASS()
class MUKSI_API UConePattern : public UAreaPattern
{
	GENERATED_BODY()

public:
	// 마지막 Step의 Origin과 Aim 방향을 기준으로 Cone 내부의 Hex 좌표를 실제 효과 범위에 추가한다.
	virtual void ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const override;

	// ConePattern이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetPatternDataStruct() const override;

private:
	// Origin에서 후보 좌표를 향하는 월드 방향이 지정한 Cone 각도 안에 있는지 확인한다.
	bool IsInsideCone(const FVector& OriginWorldLocation, const FVector& AimWorldLocation, const FVector& CandidateWorldLocation, float ConeAngle) const;
};