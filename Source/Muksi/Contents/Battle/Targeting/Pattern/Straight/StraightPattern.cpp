#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPattern.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Straight/StraightPatternData.h"

void UStraightPattern::ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData);

	const FStraightPatternData* StraightData = PatternData.GetPtr<FStraightPatternData>();
	const FTargetingStepContext* StepContext = InOutResult.GetLastStepContext();

	if (!StraightData || !StepContext || !StepContext->HasOriginCoord() || !StepContext->HasDirection() || StraightData->Range <= 0)
	{
		return;
	}

	const FHexCubeCoord Direction = FHexGridMath::GetCubeDirection(StepContext->Direction);
	FHexCubeCoord CurrentCube = FHexGridMath::OffsetToCube(StepContext->OriginCoord);

	for (int32 Distance = 1; Distance <= StraightData->Range; ++Distance)
	{
		CurrentCube = FHexCubeCoord(CurrentCube.X + Direction.X, CurrentCube.Y + Direction.Y, CurrentCube.Z + Direction.Z);

		const FHexOffsetCoord CurrentCoord = FHexGridMath::CubeToOffset(CurrentCube);

		if (!Context.GridManager->IsValidCoord(CurrentCoord))
		{
			break;
		}

		AddPathCoord(InOutResult, CurrentCoord);

		const FBattleGridCell* Cell = Context.GridManager->GetCellByCoord(CurrentCoord);

		if (!Cell || !Cell->OccupyingActor)
		{
			continue;
		}

		ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get());

		if (!TargetCharacter)
		{
			continue;
		}

		AddAffectedCoord(InOutResult, CurrentCoord);
		InOutResult.TargetCharacters.AddUnique(TargetCharacter);
		break;
	}
}

const UScriptStruct* UStraightPattern::GetPatternDataStruct() const
{
	return FStraightPatternData::StaticStruct();
}
