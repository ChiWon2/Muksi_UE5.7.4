#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

#include "Muksi/Contents/Battle/Targeting/Condition/Tree/TargetingCondNode.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Area/Data/AreaPreviewData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Path/Data/PathPreviewData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Selection/Data/SelectionPreviewData.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

#include "TargetingStepCardData.generated.h"

class UAreaPattern;
class UAreaPreviewVisualizer;
class UPathPreviewVisualizer;
class USelectionPreviewVisualizer;
class UTargetSelection;

USTRUCT(BlueprintType)
struct FTargetingPhasePresentationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Selection"))
    bool bShowSelection = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Path"))
    bool bShowPath = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Affected Area"))
    bool bShowAffectedArea = true;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Affected Highlight"))
    bool bShowAffectedHighlight = true;

    bool HasAnyPresentation() const
    {
        return bShowSelection || bShowPath || bShowAffectedArea || bShowAffectedHighlight;
    }
};

/** Selection과 Resolve가 공유하는 Step 기준점을 정의한다. */
USTRUCT(BlueprintType)
struct FTargetingOriginSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Origin", meta = (DisplayName = "Source"))
    ETargetingOriginSource Source = ETargetingOriginSource::SourceCharacter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Origin", meta = (DisplayName = "Step", EditCondition = "Source == ETargetingOriginSource::SpecificStep", EditConditionHides, ClampMin = "0"))
    int32 StepIndex = 0;
};

/** Targeting에서 Player가 어떤 Coord / Direction을 선택할 수 있는지 정의한다. */
USTRUCT(BlueprintType)
struct FTargetingSelectionSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection", meta = (DisplayName = "Rule"))
    TSubclassOf<UTargetSelection> RuleClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Selection", meta = (DisplayName = "Rule Data", BaseStruct = "/Script/Muksi.TargetSelectionData"))
    FInstancedStruct RuleData;
};

/** 확정된 Selection을 이후 World에서 다시 해석할 수 있는 Intent로 저장하는 규칙. */
USTRUCT(BlueprintType)
struct FTargetingIntentSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Intent", meta = (DisplayName = "Binding"))
    ETargetingIntentBinding Binding = ETargetingIntentBinding::OriginRelative;
};

/** 저장된 Intent를 현재 World 기준의 Coord / Direction으로 해석하는 규칙. */
USTRUCT(BlueprintType)
struct FTargetingResolveSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resolve", meta = (DisplayName = "Condition"))
    FTargetingCondNode Condition;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Resolve", meta = (DisplayName = "Invalid Policy"))
    EInvalidTargetResolvePolicy InvalidPolicy = EInvalidTargetResolvePolicy::FindNearestValid;
};

/** Selection 또는 Resolve 결과에 적용할 실제 영향 범위 규칙. */
USTRUCT(BlueprintType)
struct FTargetingPatternSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern", meta = (DisplayName = "Pattern"))
    TSubclassOf<UAreaPattern> PatternClass = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern", meta = (DisplayName = "Pattern Data", BaseStruct = "/Script/Muksi.AreaPatternData"))
    FInstancedStruct PatternData;
};

USTRUCT(BlueprintType)
struct FTargetingSelectionVisualizerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Visualizer"))
    TSubclassOf<USelectionPreviewVisualizer> Visualizer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Data", BaseStruct = "/Script/Muksi.SelectionPreviewData", EditCondition = "Visualizer != nullptr", EditConditionHides))
    FInstancedStruct Data;
};

USTRUCT(BlueprintType)
struct FTargetingPathVisualizerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Visualizer"))
    TSubclassOf<UPathPreviewVisualizer> Visualizer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Data", BaseStruct = "/Script/Muksi.PathPreviewData", EditCondition = "Visualizer != nullptr", EditConditionHides))
    FInstancedStruct Data;
};

USTRUCT(BlueprintType)
struct FTargetingAffectedAreaVisualizerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Visualizer"))
    TSubclassOf<UAreaPreviewVisualizer> Visualizer = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer", meta = (DisplayName = "Data", BaseStruct = "/Script/Muksi.AreaPreviewData", EditCondition = "Visualizer != nullptr", EditConditionHides))
    FInstancedStruct Data;
};

USTRUCT(BlueprintType)
struct FTargetingVisualizerSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizers", meta = (DisplayName = "Selection"))
    FTargetingSelectionVisualizerSettings Selection;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizers", meta = (DisplayName = "Path"))
    FTargetingPathVisualizerSettings Path;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizers", meta = (DisplayName = "Affected Area"))
    FTargetingAffectedAreaVisualizerSettings AffectedArea;
};

USTRUCT(BlueprintType)
struct FTargetingPresentationPhases
{
    GENERATED_BODY()

    FTargetingPresentationPhases()
    {
        CardReveal.bShowAffectedHighlight = false;
    }

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phases", meta = (DisplayName = "Targeting"))
    FTargetingPhasePresentationSettings Targeting;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phases", meta = (DisplayName = "Card Reveal"))
    FTargetingPhasePresentationSettings CardReveal;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phases", meta = (DisplayName = "Simulation"))
    FTargetingPhasePresentationSettings Simulation;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Phases", meta = (DisplayName = "Actual Battle"))
    FTargetingPhasePresentationSettings ActualBattle;
};

/** 계산 규칙을 소유하지 않고, 계산된 Targeting 정보를 어떤 형태와 Phase에서 보여줄지만 정의한다. */
USTRUCT(BlueprintType)
struct FTargetingStepPresentationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Visualizers"))
    FTargetingVisualizerSettings Visualizers;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (DisplayName = "Phases"))
    FTargetingPresentationPhases Phases;
};

USTRUCT(BlueprintType)
struct FTargetingStepCardData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Origin"))
    FTargetingOriginSettings Origin;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Selection"))
    FTargetingSelectionSettings Selection;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Intent"))
    FTargetingIntentSettings Intent;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Resolve"))
    FTargetingResolveSettings Resolve;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Pattern"))
    FTargetingPatternSettings Pattern;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Targeting Step", meta = (DisplayName = "Presentation"))
    FTargetingStepPresentationSettings Presentation;

#if WITH_EDITOR
    void SyncDataTypes();
    void SyncSelectionRuleDataType();
    void SyncSelectionVisualizerDataType();
    void SyncPathVisualizerDataType();
    void SyncAffectedAreaVisualizerDataType();
    void SyncPatternDataType();
#endif
};
