#include "Muksi/Contents/Battle/Targeting/Preview/Base/PathPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void UPathPreviewVisualizer::ClearPreview()
{
	if (HasPreviewActor())
	{
		GetPreviewActor()->ClearPathPreview();
	}
}

const UScriptStruct* UPathPreviewVisualizer::GetPathPreviewDataStruct() const
{
	return nullptr;
}

bool UPathPreviewVisualizer::IsPathPreviewDataValid(const FInstancedStruct& PathPreviewData) const
{
	const UScriptStruct* ExpectedStruct = GetPathPreviewDataStruct();

	if (!ExpectedStruct)
	{
		return !PathPreviewData.IsValid();
	}

	return PathPreviewData.GetScriptStruct() == ExpectedStruct;
}