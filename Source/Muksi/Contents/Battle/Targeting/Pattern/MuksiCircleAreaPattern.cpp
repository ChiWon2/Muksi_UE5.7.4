#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCircleAreaPattern.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Data/Pattern/MuksiCircleAreaPatternData.h"

void UMuksiCircleAreaPattern::ApplyAreaPattern(const FMuksiCardTargetingContext& Context, const FInstancedStruct& PatternData, FMuksiCardTargetingResult& InOutResult) const
{
	if (!Context.GridManager)
	{
		return;
	}

	if (!InOutResult.HasSelectedCoord())
	{
		return;
	}

	if (!Context.GridManager->IsValidGridCoord(InOutResult.SelectedCoord))
	{
		return;
	}

	if (!IsAreaPatternDataValid(PatternData))
	{
		return;
	}

	const FMuksiCircleAreaPatternData* Data = PatternData.GetPtr<FMuksiCircleAreaPatternData>();

	if (!Data)
	{
		return;
	}

	const int32 Radius = FMath::Max(0, Data->Radius);
	const FCubeCoord CenterCube = Context.GridManager->OddQToCube(InOutResult.SelectedCoord);

	for (int32 X = 0; X < Context.GridManager->GridWidth; ++X)
	{
		for (int32 Y = 0; Y < Context.GridManager->GridHeight; ++Y)
		{
			const FIntPoint CandidateCoord(X, Y);

			if (!Context.GridManager->IsValidGridCoord(CandidateCoord))
			{
				continue;
			}

			const FCubeCoord CandidateCube = Context.GridManager->OddQToCube(CandidateCoord);

			if (GetCubeDistance(CenterCube, CandidateCube) > Radius)
			{
				continue;
			}

			AddAffectedCoord(InOutResult, CandidateCoord);
		}
	}
}

const UScriptStruct* UMuksiCircleAreaPattern::GetAreaPatternDataStruct() const
{
	return FMuksiCircleAreaPatternData::StaticStruct();
}

int32 UMuksiCircleAreaPattern::GetCubeDistance(const FCubeCoord& A, const FCubeCoord& B) const
{
	const int32 DeltaX = FMath::Abs(A.X - B.X);
	const int32 DeltaY = FMath::Abs(A.Y - B.Y);
	const int32 DeltaZ = FMath::Abs(A.Z - B.Z);

	return FMath::Max3(DeltaX, DeltaY, DeltaZ);
}