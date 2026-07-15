#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiPointAreaPattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Data/Pattern/MuksiPointAreaPatternData.h"

void UMuksiPointAreaPattern::ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const
{
	if (!Context.GridManager)
	{
		return;
	}

	if (!InOutResult.HasSelectedCoord())
	{
		return;
	}

	if (!Context.GridManager->IsValidGridCoord(InOutResult.SelectedCoord))
	{
		return;
	}

	if (!IsAreaPatternDataValid(PatternData))
	{
		return;
	}

	AddAffectedCoord(InOutResult, InOutResult.SelectedCoord);
}

const UScriptStruct* UMuksiPointAreaPattern::GetAreaPatternDataStruct() const
{
	return FMuksiPointAreaPatternData::StaticStruct();
}