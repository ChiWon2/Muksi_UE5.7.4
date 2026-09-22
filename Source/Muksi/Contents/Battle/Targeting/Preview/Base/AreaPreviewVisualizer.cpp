#include "Muksi/Contents/Battle/Targeting/Preview/Base/AreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"

void UAreaPreviewVisualizer::ClearPreview()
{
	if (HasPreviewActor())
		GetPreviewActor()->ClearAreaPreview();
}

void UAreaPreviewVisualizer::CollectHighlightCoords(
	const FTargetingPreviewContext& Context,
	TArray<FHexOffsetCoord>& OutCoords) const
{
	static_cast<void>(Context);
	static_cast<void>(OutCoords);
}

const UScriptStruct* UAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return nullptr;
}

const UScriptStruct* UAreaPreviewVisualizer::GetAreaPreviewDataStruct() const
{
	return nullptr;
}

bool UAreaPreviewVisualizer::IsPatternDataValid(const FInstancedStruct& PatternData) const
{
	const UScriptStruct* ExpectedStruct = GetSupportedPatternDataStruct();
	return ExpectedStruct ? PatternData.GetScriptStruct() == ExpectedStruct : !PatternData.IsValid();
}

bool UAreaPreviewVisualizer::IsAreaPreviewDataValid(const FInstancedStruct& PreviewData) const
{
	const UScriptStruct* ExpectedStruct = GetAreaPreviewDataStruct();
	return ExpectedStruct ? PreviewData.GetScriptStruct() == ExpectedStruct : !PreviewData.IsValid();
}
