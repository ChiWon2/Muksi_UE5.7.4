#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

#include "TargetingStepCardData.generated.h"

class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
class UTargetSelection;

USTRUCT(BlueprintType)
struct FTargetingStepCardData
{
	GENERATED_BODY()

	// 현재 단계에서 타기팅 입력을 처리하는 Selection 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	TSubclassOf<UTargetSelection> SelectionClass = nullptr;

	// 현재 Selection 클래스가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (BaseStruct = "/Script/Muksi.TargetSelectionData"))
	FInstancedStruct SelectionData;

	// 현재 단계의 선택 가능 범위를 표시하는 Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Preview")
	TSubclassOf<USelectionPreviewVisualizer> SelectionPreviewClass = nullptr;

	// 현재 단계의 경로를 표시하는 Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Preview")
	TSubclassOf<UPathPreviewVisualizer> PathPreviewClass = nullptr;

	// 현재 PathPreview 클래스가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Preview", meta = (BaseStruct = "/Script/Muksi.PathPreviewData"))
	FInstancedStruct PathPreviewData;

	// 현재 단계의 시작 위치를 결정하는 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	ETargetingOriginSource OriginSource = ETargetingOriginSource::SourceCharacter;

	// SpecificStep 사용 시 Origin으로 사용할 이전 단계 인덱스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (EditCondition = "OriginSource == ETargetingOriginSource::SpecificStep", EditConditionHides, ClampMin = "0"))
	int32 OriginStepIndex = 0;

#if WITH_EDITOR
	// 현재 단계의 모든 InstancedStruct 타입을 동기화한다.
	void SyncDataTypes();

	// SelectionClass에 맞게 SelectionData 타입을 동기화한다.
	void SyncSelectionDataType();

	// PathPreviewClass에 맞게 PathPreviewData 타입을 동기화한다.
	void SyncPathPreviewDataType();
#endif
};