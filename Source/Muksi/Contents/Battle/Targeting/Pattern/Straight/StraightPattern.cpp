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

	const FCubeCoord Direction = Context.GridManager->GetCubeDirection(StepContext->Direction);
	FCubeCoord CurrentCube = Context.GridManager->OddQToCube(StepContext->OriginCoord);

	for (int32 Distance = 1; Distance <= StraightData->Range; ++Distance)
	{
		CurrentCube = FCubeCoord(CurrentCube.X + Direction.X, CurrentCube.Y + Direction.Y, CurrentCube.Z + Direction.Z);

		const FIntPoint CurrentCoord = Context.GridManager->CubeToOddQ(CurrentCube);

		if (!Context.GridManager->IsValidGridCoord(CurrentCoord))
		{
			break;
		}

		AddPathCoord(InOutResult, CurrentCoord);

		const FBattleGridCell* Cell = Context.GridManager->GetCell(CurrentCoord);

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