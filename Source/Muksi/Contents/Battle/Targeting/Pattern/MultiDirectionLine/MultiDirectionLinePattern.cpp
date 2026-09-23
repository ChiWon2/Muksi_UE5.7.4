#include "Muksi/Contents/Battle/Targeting/Pattern/MultiDirectionLine/MultiDirectionLinePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/MultiDirectionLine/MultiDirectionLinePatternData.h"

void UMultiDirectionLinePattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType, const FInstancedStruct& PatternData, const FHexOffsetCoord& OriginCoord, const FHexOffsetCoord&, int32 Direction, TArray<FTargetingGroup>& OutGroups) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FMultiDirectionLinePatternData* Data = PatternData.GetPtr<FMultiDirectionLinePatternData>();

	if (!Data || !GridManager->IsValidCoord(OriginCoord) || Direction == INDEX_NONE || Data->Range <= 0)
		return;

	for (const int32 DirectionOffset : Data->DirectionOffsets)
	{
		FTargetingGroup& Group = OutGroups.AddDefaulted_GetRef();
		const int32 GroupDirection = FHexGridMath::NormalizeDirection(Direction + DirectionOffset);
		Group.Direction = GroupDirection;

		for (int32 Distance = 1; Distance <= Data->Range; ++Distance)
		{
			const FHexOffsetCoord Coord = FHexGridMath::GetNeighborCoord(OriginCoord, GroupDirection, Distance);

			if (!GridManager->IsValidCoord(Coord))
				break;

			AddPathCoord(Group.PathCoords, Coord);
			AddAffectedCoord(Group.AffectedCoords, Coord);
		}
	}
}

const UScriptStruct* UMultiDirectionLinePattern::GetPatternDataStruct() const
{
	return FMultiDirectionLinePatternData::StaticStruct();
}
