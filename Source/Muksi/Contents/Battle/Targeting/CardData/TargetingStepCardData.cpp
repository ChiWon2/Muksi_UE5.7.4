#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#if WITH_EDITOR

void FTargetingStepCardData::SyncDataTypes()
{
	SyncSelectionDataType();
	SyncPathPreviewDataType();
}

void FTargetingStepCardData::SyncSelectionDataType()
{
	if (!SelectionClass)
	{
		SelectionData.Reset();
		return;
	}

	const UTargetSelection* SelectionCDO = SelectionClass.GetDefaultObject();

	if (!SelectionCDO)
	{
		SelectionData.Reset();
		return;
	}

	const UScriptStruct* ExpectedDataStruct = SelectionCDO->GetSelectionDataStruct();

	if (!ExpectedDataStruct)
	{
		SelectionData.Reset();
		return;
	}

	if (SelectionData.GetScriptStruct() == ExpectedDataStruct)
	{
		return;
	}

	SelectionData.InitializeAs(ExpectedDataStruct);
}

void FTargetingStepCardData::SyncPathPreviewDataType()
{
	if (!PathPreviewClass)
	{
		PathPreviewData.Reset();
		return;
	}

	const UPathPreviewVisualizer* PathPreviewCDO = PathPreviewClass.GetDefaultObject();

	if (!PathPreviewCDO)
	{
		PathPreviewData.Reset();
		return;
	}

	const UScriptStruct* ExpectedDataStruct = PathPreviewCDO->GetPathPreviewDataStruct();

	if (!ExpectedDataStruct)
	{
		PathPreviewData.Reset();
		return;
	}

	if (PathPreviewData.GetScriptStruct() == ExpectedDataStruct)
	{
		return;
	}

	PathPreviewData.InitializeAs(ExpectedDataStruct);
}

#endif