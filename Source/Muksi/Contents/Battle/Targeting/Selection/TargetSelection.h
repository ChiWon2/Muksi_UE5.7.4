#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelectionContext.h"

#include "TargetSelection.generated.h"

UCLASS(Abstract)
class MUKSI_API UTargetSelection : public UObject
{
	GENERATED_BODY()

public:
	// 현재 입력과 SelectionData를 사용해 현재 Step 상태를 계산한다.
	virtual void Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const;

	// 현재 Selection이 사용하는 설정 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetSelectionDataStruct() const;

protected:
	// 전달된 SelectionData가 현재 Selection이 요구하는 구조체 타입인지 확인한다.
	bool IsSelectionDataValid(const FInstancedStruct& SelectionData) const;

	// 현재 Selection 실행 환경을 기준으로 StepContext의 공통 값을 초기화한다.
	void InitializeStepContext(const FTargetSelectionContext& Context, FTargetingStepContext& OutStepContext) const;
};



#define TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(Context, SelectionData) \
	do \
	{ \
		if (!(Context).GridManager) \
		{ \
			return; \
		} \
		if (!(Context).HasOriginCoord()) \
		{ \
			return; \
		} \
		if (!(Context).GridManager->IsValidCoord((Context).OriginCoord)) \
		{ \
			return; \
		} \
		if (!IsSelectionDataValid(SelectionData)) \
		{ \
			return; \
		} \
	} \
	while (false)

#define TARGET_SELECTION_VALIDATE_HOVERED_COORD_OR_RETURN(Context) \
	do \
	{ \
		if (!(Context).InputContext.HasHoveredCoord()) \
		{ \
			return; \
		} \
		if (!(Context).GridManager->IsValidCoord((Context).InputContext.HoveredCoord)) \
		{ \
			return; \
		} \
	} \
	while (false)

