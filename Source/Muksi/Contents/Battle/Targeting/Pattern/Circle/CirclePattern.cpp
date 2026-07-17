#include "Muksi/Contents/Battle/Targeting/Pattern/Circle/CirclePattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/Circle/CirclePatternData.h"

void UCirclePattern::ApplyPattern(const FAreaPatternContext& Context, const FInstancedStruct& PatternData, FTargetingResult& InOutResult) const
{
	AREA_PATTERN_VALIDATE_COMMON_OR_RETURN(Context, PatternData);

	const FCirclePatternData* Data = PatternData.GetPtr<FCirclePatternData>();
	const FTargetingStepContext* StepContext = InOutResult.GetLastStepContext();

	if (!Data || !StepContext || !StepContext->HasSelectedCoord())
	{
		return;
	}

	const FIntPoint CenterCoord = StepContext->SelectedCoord;

	if (!Context.GridManager->IsValidGridCoord(CenterCoord))
	{
		return;
	}

	const int32 SafeRadius = FMath::Max(0, Data->Radius);

	for (int32 X = 0; X < Context.GridManager->GridWidth; ++X)
	{
		for (int32 Y = 0; Y < Context.GridManager->GridHeight; ++Y)
		{
			const FIntPoint CandidateCoord(X, Y);

			if (!Context.GridManager->IsValidGridCoord(CandidateCoord))
			{
				continue;
			}

			const int32 HexDistance = FHexGridMath::GetHexDistance(CenterCoord, CandidateCoord);

			if (HexDistance > SafeRadius)
			{
				continue;
			}

			if (!Data->bIncludeCenterCoord && CandidateCoord == CenterCoord)
			{
				continue;
			}

			AddAffectedCoord(InOutResult, CandidateCoord);
		}
	}
}

const UScriptStruct* UCirclePattern::GetPatternDataStruct() const
{
	return FCirclePatternData::StaticStruct();
}