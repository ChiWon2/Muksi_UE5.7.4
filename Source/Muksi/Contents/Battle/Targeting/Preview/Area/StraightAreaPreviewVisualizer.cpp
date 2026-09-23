#include "Muksi/Contents/Battle/Targeting/Preview/Area/StraightAreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UStraightAreaPreviewVisualizer::CollectHighlightCoords(
	const FTargetingPreviewContext& Context,
	TArray<FHexOffsetCoord>& OutCoords) const
{
	if (!Context.TargetingStep)
		return;

	for (const FTargetingGroup& Group : Context.TargetingStep->Groups)
	{
		for (const FHexOffsetCoord& Coord : Group.PathCoords)
			OutCoords.AddUnique(Coord);
	}
}
