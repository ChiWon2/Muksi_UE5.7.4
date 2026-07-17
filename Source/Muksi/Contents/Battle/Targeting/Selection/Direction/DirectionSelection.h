#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "DirectionSelection.generated.h"

UCLASS()
class MUKSI_API UDirectionSelection : public UTargetSelection
{
	GENERATED_BODY()

public:
	// Origin에서 AimWorldLocation을 향하는 방향을 계산해 StepContext에 저장한다.
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const override;

	// DirectionSelection이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetSelectionDataStruct() const override;

private:
	// 실제 배치된 인접 타일의 월드 방향과 Aim 방향을 비교해 가장 가까운 Hex 방향을 반환한다.
	int32 ResolveHexDirection(const FTargetSelectionContext& Context, const FVector& AimDirection) const;
};