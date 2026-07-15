#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"

void UMuksiCardAreaPattern::ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const
{
}

const UScriptStruct* UMuksiCardAreaPattern::GetAreaPatternDataStruct() const
{
	return nullptr;
}

bool UMuksiCardAreaPattern::IsAreaPatternDataValid(const FInstancedStruct& PatternData) const
{
	const UScriptStruct* ExpectedStruct = GetAreaPatternDataStruct();

	if (!ExpectedStruct)
	{
		return !PatternData.IsValid();
	}

	return PatternData.GetScriptStruct() == ExpectedStruct;
}

void UMuksiCardAreaPattern::AddAffectedCoord(FMuksiCardTargetingResult& InOutResult, const FIntPoint& Coord) const
{
	InOutResult.AffectedCoords.AddUnique(Coord);
}

void UMuksiCardAreaPattern::AddPathCoord(FMuksiCardTargetingResult& InOutResult, const FIntPoint& Coord) const
{
	InOutResult.PathCoords.AddUnique(Coord);
}