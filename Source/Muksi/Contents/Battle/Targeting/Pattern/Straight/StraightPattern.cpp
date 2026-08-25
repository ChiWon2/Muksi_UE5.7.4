#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPattern.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPatternData.h"

void UStraightPattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord&, int32 Direction, TArray<FHexOffsetCoord>& OutAffectedCoords, TArray<FHexOffsetCoord>& OutPathCoords) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FStraightPatternData* StraightData = PatternData.GetPtr<FStraightPatternData>();
	if (!StraightData || !GridManager->IsValidCoord(OriginCoord) || Direction == INDEX_NONE || StraightData->Range <= 0) return;

	const FHexCubeCoord HexDirection = FHexGridMath::GetCubeDirection(Direction);
	FHexCubeCoord CurrentCube = FHexGridMath::OffsetToCube(OriginCoord);

	for (int32 Distance = 1; Distance <= StraightData->Range; ++Distance)
	{
		CurrentCube = FHexCubeCoord(CurrentCube.X + HexDirection.X, CurrentCube.Y + HexDirection.Y, CurrentCube.Z + HexDirection.Z);
		const FHexOffsetCoord CurrentCoord = FHexGridMath::CubeToOffset(CurrentCube);
		if (!GridManager->IsValidCoord(CurrentCoord)) break;

		AddPathCoord(OutPathCoords, CurrentCoord);
		const FBattleGridCell* Cell = GridManager->GetCellByCoord(WorldType, CurrentCoord);
		if (!Cell || !Cell->OccupyingActor) continue;
		if (!Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get())) continue;
		AddAffectedCoord(OutAffectedCoords, CurrentCoord);
		break;
	}
}

const UScriptStruct* UStraightPattern::GetPatternDataStruct() const
{
	return FStraightPatternData::StaticStruct();
}
