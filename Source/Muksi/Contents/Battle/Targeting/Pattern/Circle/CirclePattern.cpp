#include "Muksi/Contents/Battle/Targeting/Pattern/Circle/CirclePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Circle/CirclePatternData.h"

void UCirclePattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord&, const FHexOffsetCoord& TargetCoord, int32, TArray<FHexOffsetCoord>& OutAffectedCoords, TArray<FHexOffsetCoord>&) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FCirclePatternData* Data = PatternData.GetPtr<FCirclePatternData>();
	if (!Data || !GridManager->IsValidCoord(TargetCoord)) return;

	const int32 SafeRadius = FMath::Max(0, Data->Radius);
	for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
	{
		for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
		{
			const FHexOffsetCoord CandidateCoord(X, Y);
			if (!GridManager->IsValidCoord(CandidateCoord)) continue;
			if (FHexGridMath::GetHexDistance(TargetCoord, CandidateCoord) > SafeRadius) continue;
			if (!Data->bIncludeCenterCoord && CandidateCoord == TargetCoord) continue;
			AddAffectedCoord(OutAffectedCoords, CandidateCoord);
		}
	}
}

const UScriptStruct* UCirclePattern::GetPatternDataStruct() const
{
	return FCirclePatternData::StaticStruct();
}
