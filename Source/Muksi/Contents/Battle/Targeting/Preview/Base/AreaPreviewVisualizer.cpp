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
