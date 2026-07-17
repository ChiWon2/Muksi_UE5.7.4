#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

void UAreaPattern::ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const
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

void UAreaPattern::AddAffectedCoord(FTargetingResult& InOutResult, const FIntPoint& Coord) const
{
	InOutResult.AffectedCoords.AddUnique(Coord);
}

void UAreaPattern::AddPathCoord(FTargetingResult& InOutResult, const FIntPoint& Coord) const
{
	InOutResult.PathCoords.AddUnique(Coord);
}