#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPattern.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPatternData.h"

void UStraightPattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType WorldType, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FStraightPatternData* StraightData = PatternData.GetPtr<FStraightPatternData>();
	const FTargetingStepResult* StepResult = InOutResult.GetLastStep();

	if (!StraightData || !StepResult || !StepResult->HasOriginCoord() || !StepResult->HasDirection() || StraightData->Range <= 0)
	{
		return;
	}

	const FHexCubeCoord Direction = FHexGridMath::GetCubeDirection(StepResult->Direction);
	FHexCubeCoord CurrentCube = FHexGridMath::OffsetToCube(StepResult->OriginCoord);

	for (int32 Distance = 1; Distance <= StraightData->Range; ++Distance)
	{
		CurrentCube = FHexCubeCoord(CurrentCube.X + Direction.X, CurrentCube.Y + Direction.Y, CurrentCube.Z + Direction.Z);

		const FHexOffsetCoord CurrentCoord = FHexGridMath::CubeToOffset(CurrentCube);

		if (!GridManager->IsValidCoord(CurrentCoord))
		{
			break;
		}

		AddPathCoord(InOutResult, CurrentCoord);

		const FBattleGridCell* Cell = GridManager->GetCellByCoord(WorldType, CurrentCoord);

		if (!Cell || !Cell->OccupyingActor)
		{
			continue;
		}

		if (!Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()))
		{
			continue;
		}

		AddAffectedCoord(InOutResult, CurrentCoord);
		break;
	}
}

const UScriptStruct* UStraightPattern::GetPatternDataStruct() const
{
	return FStraightPatternData::StaticStruct();
}
