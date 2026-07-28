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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection")
	TSubclassOf<UTargetSelection> SelectionClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection", meta = (BaseStruct = "/Script/Muksi.TargetSelectionData"))
	FInstancedStruct SelectionData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Selection")
	TSubclassOf<USelectionPreviewVisualizer> SelectionPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Path")
	TSubclassOf<UPathPreviewVisualizer> PathPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Path", meta = (BaseStruct = "/Script/Muksi.PathPreviewData"))
	FInstancedStruct PathPreviewData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	TSubclassOf<UAreaPattern> PatternClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern", meta = (BaseStruct = "/Script/Muksi.AreaPatternData"))
	FInstancedStruct PatternData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	TSubclassOf<UAreaPreviewVisualizer> AreaPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Pattern")
	ETargetingGridPreviewMode GridPreviewMode = ETargetingGridPreviewMode::AffectedTiles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Origin")
	ETargetingOriginSource OriginSource = ETargetingOriginSource::SourceCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step|Origin", meta = (EditCondition = "OriginSource == ETargetingOriginSource::SpecificStep", EditConditionHides, ClampMin = "0"))
	int32 OriginStepIndex = 0;

#if WITH_EDITOR
	void SyncDataTypes();
	void SyncSelectionDataType();
	void SyncPathPreviewDataType();
	void SyncPatternDataType();
#endif
};