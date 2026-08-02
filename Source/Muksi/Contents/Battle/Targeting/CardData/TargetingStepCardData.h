#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingCondNode.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingGridPreviewMode.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

#include "TargetingStepCardData.generated.h"

class UAreaPattern;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
class UTargetSelection;

/** 타겟팅 단계의 시점별 조건 Tree. */
USTRUCT(BlueprintType)
struct FTargetingConditionSettings
{
	GENERATED_BODY()

	/** 현재 입력 시점의 후보 좌표 조건. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	FTargetingCondNode Selection;

	/** Simulation/Resolver가 Intent를 현재 Grid에 다시 적용할 때의 조건. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	FTargetingCondNode Resolve;

	/** 실제 Execution 직전 최종 검증용 조건. 현재는 데이터 예약 필드다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Conditions")
	FTargetingCondNode Execution;
};

/** Simulation/Attack Sequence에서 Step별로 표시할 Preview 채널. */
USTRUCT(BlueprintType)
struct FTargetingPhasePresentationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bShowSelectionPreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bShowPathPreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bShowAreaPreview = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bShowIndicator = true;
};

/** Targeting 입력 이후의 두 런타임 Phase별 표시 설정. */
USTRUCT(BlueprintType)
struct FTargetingPresentationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Simulation Phase"))
	FTargetingPhasePresentationSettings SimulationPhase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Attack Sequence Phase"))
	FTargetingPhasePresentationSettings AttackSequencePhase;
};

/** 일반 카드 제작에서는 자주 변경하지 않는 선택/재해석/표시 정책. */
USTRUCT(BlueprintType)
struct FTargetingAdvancedSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	ETargetingIntentBinding IntentBinding = ETargetingIntentBinding::SourceRelative;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	EInvalidTargetResolvePolicy InvalidResolvePolicy = EInvalidTargetResolvePolicy::FindNearestValid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced")
	FTargetingPresentationSettings Presentation;
};

/** 후보 좌표를 선택하고, 이후 시뮬레이션에서 다시 해석하기 위한 설정. */
USTRUCT(BlueprintType)
struct FTargetingSelectionSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection")
	TSubclassOf<UTargetSelection> SelectionClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection", meta = (BaseStruct = "/Script/Muksi.TargetSelectionData"))
	FInstancedStruct SelectionData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection|Origin")
	ETargetingOriginSource OriginSource = ETargetingOriginSource::SourceCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection|Origin", meta = (EditCondition = "OriginSource == ETargetingOriginSource::SpecificStep", EditConditionHides, ClampMin = "0"))
	int32 OriginStepIndex = 0;

};

/** 선택 결과로부터 실제 영향 좌표 집합을 계산하기 위한 설정. */
USTRUCT(BlueprintType)
struct FTargetingPatternSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern")
	TSubclassOf<UAreaPattern> PatternClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern", meta = (BaseStruct = "/Script/Muksi.AreaPatternData"))
	FInstancedStruct PatternData;
};

/** 선택/경로/영향 범위 표시를 담당하는 시각화 설정. */
USTRUCT(BlueprintType)
struct FTargetingPreviewSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview|Selection")
	TSubclassOf<USelectionPreviewVisualizer> SelectionPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview|Area")
	TSubclassOf<UAreaPreviewVisualizer> AreaPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview|Path")
	TSubclassOf<UPathPreviewVisualizer> PathPreviewClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview|Path", meta = (BaseStruct = "/Script/Muksi.PathPreviewData"))
	FInstancedStruct PathPreviewData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Preview|Grid")
	ETargetingGridPreviewMode GridPreviewMode = ETargetingGridPreviewMode::AffectedTiles;
};

USTRUCT(BlueprintType)
struct FTargetingStepCardData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	FTargetingSelectionSettings Selection;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	FTargetingPatternSettings Pattern;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	FTargetingPreviewSettings Preview;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step")
	FTargetingConditionSettings Conditions;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Advanced Settings"))
	FTargetingAdvancedSettings AdvancedSettings;

#if WITH_EDITOR
	void SyncDataTypes();
	void SyncSelectionDataType();
	void SyncPathPreviewDataType();
	void SyncPatternDataType();
#endif
};
