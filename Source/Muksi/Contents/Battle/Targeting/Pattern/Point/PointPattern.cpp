#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"

void UPointPattern::ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData);

	const FTargetingStepContext* StepContext = InOutResult.GetLastStepContext();

	if (!StepContext || !StepContext->HasSelectedCoord())
	{
		return;
	}

	if (!Context.GridManager->IsValidGridCoord(StepContext->SelectedCoord))
	{
		return;
	}

	AddAffectedCoord(InOutResult, StepContext->SelectedCoord);
}

const UScriptStruct* UPointPattern::GetPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}