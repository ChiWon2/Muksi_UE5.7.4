#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#if WITH_EDITOR

void FTargetingStepCardData::SyncDataTypes()
{
	SyncSelectionDataType();
	SyncPathPreviewDataType();
	SyncPatternDataType();
}

void FTargetingStepCardData::SyncSelectionDataType()
{
	if (!Selection.SelectionClass)
	{
		Selection.SelectionData.Reset();
		return;
	}

	const UTargetSelection* SelectionCDO = Selection.SelectionClass.GetDefaultObject();
	const UScriptStruct* ExpectedDataStruct = SelectionCDO ? SelectionCDO->GetSelectionDataStruct() : nullptr;

	if (!ExpectedDataStruct)
	{
		Selection.SelectionData.Reset();
		return;
	}

	if (Selection.SelectionData.GetScriptStruct() != ExpectedDataStruct)
	{
		Selection.SelectionData.InitializeAs(ExpectedDataStruct);
	}
}

void FTargetingStepCardData::SyncPathPreviewDataType()
{
	if (!Preview.PathPreviewClass)
	{
		Preview.PathPreviewData.Reset();
		return;
	}

	const UPathPreviewVisualizer* PathPreviewCDO = Preview.PathPreviewClass.GetDefaultObject();
	const UScriptStruct* ExpectedDataStruct = PathPreviewCDO ? PathPreviewCDO->GetPathPreviewDataStruct() : nullptr;

	if (!ExpectedDataStruct)
	{
		Preview.PathPreviewData.Reset();
		return;
	}

	if (Preview.PathPreviewData.GetScriptStruct() != ExpectedDataStruct)
	{
		Preview.PathPreviewData.InitializeAs(ExpectedDataStruct);
	}
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
	{
		Pattern.PatternData.InitializeAs(ExpectedDataStruct);
	}
}

#endif
