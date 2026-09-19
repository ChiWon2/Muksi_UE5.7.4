#include "Muksi/Contents/Battle/Targeting/Preview/Area/StraightAreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UStraightAreaPreviewVisualizer::CollectHighlightCoords(
	const FTargetingPreviewContext& Context,
	TArray<FHexOffsetCoord>& OutCoords) const
{
	if (!Context.IsValid())
		return;

	if (!IsPatternDataValid(Context.StepData->Pattern.PatternData))
		return;

	const TArray<FHexOffsetCoord>* PathCoords = Context.GetPathCoords();

	if (!PathCoords)
		return;

	for (const FHexOffsetCoord& Coord : *PathCoords)
		OutCoords.AddUnique(Coord);
}

const UScriptStruct* UStraightAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FStraightPatternData::StaticStruct();
}
