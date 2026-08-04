#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

void UAreaPattern::ApplyPattern(ABattleGridManager* GridManager, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const
{
}

const UScriptStruct* UAreaPattern::GetPatternDataStruct() const
{
	return nullptr;
}

bool UAreaPattern::IsPatternDataValid(const FInstancedStruct& PatternData) const
{
	const UScriptStruct* ExpectedStruct = GetPatternDataStruct();

	if (!ExpectedStruct)
	{
		return !PatternData.IsValid();
	}

	return PatternData.GetScriptStruct() == ExpectedStruct;
}

void UAreaPattern::AddAffectedCoord(FResolvedTargeting& InOutResult, const FHexOffsetCoord& Coord) const
{
	InOutResult.AffectedCoords.AddUnique(Coord);
}

void UAreaPattern::AddPathCoord(FResolvedTargeting& InOutResult, const FHexOffsetCoord& Coord) const
{
	InOutResult.PathCoords.AddUnique(Coord);
}
