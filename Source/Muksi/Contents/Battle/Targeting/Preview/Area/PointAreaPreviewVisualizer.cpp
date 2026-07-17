#include "Muksi/Contents/Battle/Targeting/Preview/Area/PointAreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UPointAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.TargetingData || !Context.PreviewResult)
	{
		return;
	}

	if (!IsPatternDataValid(Context.TargetingData->FinalPatternData))
	{
		return;
	}

	GetPreviewActor()->SetAreaGridCoords(Context.PreviewResult->AffectedCoords);
}

const UScriptStruct* UPointAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}