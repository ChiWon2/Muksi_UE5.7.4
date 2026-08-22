#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Point/PointPatternData.h"

void UPointPattern::ApplyPattern(ABattleGridManager* GridManager, EBattleSimulationWorldType, const FInstancedStruct& PatternData, FResolvedTargeting& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(GridManager, PatternData);

	const FTargetingStepResult* StepResult = InOutResult.GetLastStep();

	if (!StepResult || !StepResult->HasSelectedCoord())
	{
		return;
	}

	if (!GridManager->IsValidCoord(StepResult->SelectedCoord))
	{
		return;
	}

	AddAffectedCoord(InOutResult, StepResult->SelectedCoord);
}

const UScriptStruct* UPointPattern::GetPatternDataStruct() const
{
	return FPointPatternData::StaticStruct();
}
