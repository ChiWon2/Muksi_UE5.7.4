#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Types/TargetingGridPreviewMode.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

#include "TargetingStepCardData.generated.h"

class UAreaPattern;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
class UTargetSelection;

USTRUCT(BlueprintType)
struct FTargetingStepCardData
{
	GENERATED_BODY()

	// 현재 단계에서 타기팅 입력을 처리하는 Selection 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection")
	TSubclassOf<UTargetSelection> SelectionClass = nullptr;

	// 현재 Selection 클래스가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection", meta = (BaseStruct = "/Script/Muksi.TargetSelectionData"))
	FInstancedStruct SelectionData;

	// 현재 단계의 선택 범위를 표시하는 Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection")
	TSubclassOf<USelectionPreviewVisualizer> SelectionPreviewClass = nullptr;

	// 현재 단계의 경로를 표시하는 Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Path")
	TSubclassOf<UPathPreviewVisualizer> PathPreviewClass = nullptr;

	// 현재 PathPreview 클래스가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Path", meta = (BaseStruct = "/Script/Muksi.PathPreviewData"))
	FInstancedStruct PathPreviewData;

	// 현재 단계의 효과 범위를 계산하는 Pattern 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	TSubclassOf<UAreaPattern> PatternClass = nullptr;

	// 현재 Pattern 클래스가 사용하는 설정 데이터.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern", meta = (BaseStruct = "/Script/Muksi.AreaPatternData"))
	FInstancedStruct PatternData;

	// 현재 단계의 Pattern 범위를 표시하는 Preview 클래스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	TSubclassOf<UAreaPreviewVisualizer> AreaPreviewClass = nullptr;

	// 현재 단계에서 효과 좌표와 경로 좌표를 표시하는 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	ETargetingGridPreviewMode GridPreviewMode = ETargetingGridPreviewMode::AffectedTiles;

	// 현재 단계의 시작 위치를 결정하는 방식.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Origin")
	ETargetingOriginSource OriginSource = ETargetingOriginSource::SourceCharacter;

	// SpecificStep 사용 시 Origin으로 사용할 이전 단계 인덱스.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Origin", meta = (EditCondition = "OriginSource == ETargetingOriginSource::SpecificStep", EditConditionHides, ClampMin = "0"))
	int32 OriginStepIndex = 0;

#if WITH_EDITOR
	void SyncDataTypes();
	void SyncSelectionDataType();
	void SyncPathPreviewDataType();
	void SyncPatternDataType();
#endif
};