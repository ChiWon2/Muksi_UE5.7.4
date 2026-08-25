#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"

void UPointPattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord&, const FHexOffsetCoord& TargetCoord, int32, TArray<FHexOffsetCoord>& OutAffectedCoords, TArray<FHexOffsetCoord>&) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);
	if (!GridManager->IsValidCoord(TargetCoord)) return;
	AddAffectedCoord(OutAffectedCoords, TargetCoord);
}

const UScriptStruct* UPointPattern::GetPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}
