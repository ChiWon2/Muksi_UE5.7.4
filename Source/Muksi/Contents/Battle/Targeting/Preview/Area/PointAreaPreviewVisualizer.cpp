#include "Muksi/Contents/Battle/Targeting/Preview/Area/PointAreaPreviewVisualizer.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingStepCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Actor/TargetingPreviewActor.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

void UPointAreaPreviewVisualizer::UpdatePreview(const FTargetingPreviewContext& Context)
{
	ClearPreview();

	if (!HasPreviewActor() || !Context.IsValid() || !Context.ResolvedTargeting)
	{
		return;
	}

	if (!IsPatternDataValid(Context.StepData->Pattern.PatternData))
	{
		return;
	}

	GetPreviewActor()->SetAreaGridCoords(Context.ResolvedTargeting->AffectedCoords);
}

const UScriptStruct* UPointAreaPreviewVisualizer::GetSupportedPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}
