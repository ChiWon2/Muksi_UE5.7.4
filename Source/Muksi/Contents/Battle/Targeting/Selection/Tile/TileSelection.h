#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "TileSelection.generated.h"

UCLASS()
class MUKSI_API UTileSelection : public UTargetSelection
{
	GENERATED_BODY()

public:
	// 현재 호버 타일이 Origin 기준 SelectionRange 안에 있으면 선택 가능한 StepContext를 생성한다.
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const override;

	// TileSelection이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetSelectionDataStruct() const override;
};