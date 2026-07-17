#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingGridPreviewMode.h"

#include "TargetingCardData.generated.h"

class UAreaPreviewVisualizer;

USTRUCT(BlueprintType)
struct FTargetingCardData
{
	GENERATED_BODY()

	// 순서대로 진행하는 Targeting Step 목록.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepCardData> Steps;

	// 모든 Step 확정 후 최종 효과 범위를 계산하는 Pattern 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting")
	TSubclassOf<UAreaPattern> FinalPatternClass = nullptr;

	// 현재 FinalPatternClass가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting", meta = (BaseStruct = "/Script/Muksi.AreaPatternData"))
	FInstancedStruct FinalPatternData;

	// 최종 효과 범위를 표시하는 Area Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting|Preview")
	TSubclassOf<UAreaPreviewVisualizer> AreaPreviewClass = nullptr;

	// 최종 효과 좌표와 경로 좌표를 그리드에 표시하는 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting|Preview")
	ETargetingGridPreviewMode GridPreviewMode = ETargetingGridPreviewMode::AffectedTiles;

	bool HasSteps() const
	{
		return !Steps.IsEmpty();
	}

	bool IsValidStepIndex(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex);
	}

	const FTargetingStepCardData* GetStep(int32 StepIndex) const
	{
		return Steps.IsValidIndex(StepIndex) ? &Steps[StepIndex] : nullptr;
	}

	bool HasFinalPattern() const
	{
		return FinalPatternClass != nullptr;
	}

#if WITH_EDITOR
	void SyncDataTypes();
	void SyncStepDataTypes();
	void SyncPatternDataType();
#endif
};