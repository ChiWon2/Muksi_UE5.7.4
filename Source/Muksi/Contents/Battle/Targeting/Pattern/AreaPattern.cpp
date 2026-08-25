#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"

void UAreaPattern::ApplyPattern(
	ABattleGridManager*,
	EBattleSimulationWorldType,
	const FInstancedStruct&,
	const FHexOffsetCoord&,
	const FHexOffsetCoord&,
	int32,
	TArray<FHexOffsetCoord>&,
	TArray<FHexOffsetCoord>&) const
{
}

const UScriptStruct* UAreaPattern::GetPatternDataStruct() const
{
	return nullptr;
}

bool UAreaPattern::IsPatternDataValid(const FInstancedStruct& PatternData) const
{
	const UScriptStruct* ExpectedStruct = GetPatternDataStruct();
	return ExpectedStruct ? PatternData.GetScriptStruct() == ExpectedStruct : !PatternData.IsValid();
}

void UAreaPattern::AddAffectedCoord(TArray<FHexOffsetCoord>& OutAffectedCoords, const FHexOffsetCoord& Coord) const
{
	OutAffectedCoords.AddUnique(Coord);
}

void UAreaPattern::AddPathCoord(TArray<FHexOffsetCoord>& OutPathCoords, const FHexOffsetCoord& Coord) const
{
	OutPathCoords.AddUnique(Coord);
}
