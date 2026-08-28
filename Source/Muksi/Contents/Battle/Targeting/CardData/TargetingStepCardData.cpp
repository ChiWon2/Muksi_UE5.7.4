#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/SelectionPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#if WITH_EDITOR

void FTargetingStepCardData::SyncDataTypes()
{
    SyncSelectionRuleDataType();
    SyncSelectionVisualizerDataType();
    SyncPathVisualizerDataType();
    SyncAffectedAreaVisualizerDataType();
    SyncPatternDataType();
}

void FTargetingStepCardData::SyncSelectionRuleDataType()
{
    if (!Selection.RuleClass)
    {
        Selection.RuleData.Reset();
        return;
    }

    const UTargetSelection* SelectionCDO = Selection.RuleClass.GetDefaultObject();
    const UScriptStruct* ExpectedDataStruct = SelectionCDO ? SelectionCDO->GetRuleDataStruct() : nullptr;

    if (!ExpectedDataStruct)
    {
        Selection.RuleData.Reset();
        return;
    }

    if (Selection.RuleData.GetScriptStruct() != ExpectedDataStruct)
        Selection.RuleData.InitializeAs(ExpectedDataStruct);
}

void FTargetingStepCardData::SyncSelectionVisualizerDataType()
{
    FTargetingSelectionVisualizerSettings& Settings = Presentation.Visualizers.Selection;
    if (!Settings.Visualizer)
    {
        Settings.Data.Reset();
        return;
    }

    const USelectionPreviewVisualizer* VisualizerCDO = Settings.Visualizer.GetDefaultObject();
    const UScriptStruct* ExpectedDataStruct = VisualizerCDO ? VisualizerCDO->GetSelectionPreviewDataStruct() : nullptr;

    if (!ExpectedDataStruct)
    {
        Settings.Data.Reset();
        return;
    }

    if (Settings.Data.GetScriptStruct() != ExpectedDataStruct)
        Settings.Data.InitializeAs(ExpectedDataStruct);
}

void FTargetingStepCardData::SyncPathVisualizerDataType()
{
    FTargetingPathVisualizerSettings& Settings = Presentation.Visualizers.Path;
    if (!Settings.Visualizer)
    {
        Settings.Data.Reset();
        return;
    }

    const UPathPreviewVisualizer* VisualizerCDO = Settings.Visualizer.GetDefaultObject();
    const UScriptStruct* ExpectedDataStruct = VisualizerCDO ? VisualizerCDO->GetPathPreviewDataStruct() : nullptr;

    if (!ExpectedDataStruct)
    {
        Settings.Data.Reset();
        return;
    }

    if (Settings.Data.GetScriptStruct() != ExpectedDataStruct)
        Settings.Data.InitializeAs(ExpectedDataStruct);
}

void FTargetingStepCardData::SyncAffectedAreaVisualizerDataType()
{
    FTargetingAffectedAreaVisualizerSettings& Settings = Presentation.Visualizers.AffectedArea;
    if (!Settings.Visualizer)
    {
        Settings.Data.Reset();
        return;
    }

    const UAreaPreviewVisualizer* VisualizerCDO = Settings.Visualizer.GetDefaultObject();
    const UScriptStruct* ExpectedDataStruct = VisualizerCDO ? VisualizerCDO->GetAreaPreviewDataStruct() : nullptr;

    if (!ExpectedDataStruct)
    {
        Settings.Data.Reset();
        return;
    }

    if (Settings.Data.GetScriptStruct() != ExpectedDataStruct)
        Settings.Data.InitializeAs(ExpectedDataStruct);
}

void FTargetingStepCardData::SyncPatternDataType()
{
    if (!Pattern.PatternClass)
    {
        Pattern.PatternData.Reset();
        return;
    }

    const UAreaPattern* PatternCDO = Pattern.PatternClass.GetDefaultObject();
    const UScriptStruct* ExpectedDataStruct = PatternCDO ? PatternCDO->GetPatternDataStruct() : nullptr;

    if (!ExpectedDataStruct)
    {
        Pattern.PatternData.Reset();
        return;
    }

    if (Pattern.PatternData.GetScriptStruct() != ExpectedDataStruct)
        Pattern.PatternData.InitializeAs(ExpectedDataStruct);
}

#endif
