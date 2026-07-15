#include "Muksi/Contents/Battle/Targeting/Policy/MuksiTileTargetingPolicy.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Data/Policy/MuksiTileTargetingData.h"

void UMuksiTileTargetingPolicy::BuildTargetingResult(const FMuksiCardTargetingContext& Context, const FInstancedStruct& TargetingData, FMuksiCardTargetingResult& OutResult) const
{
	InitializeResult(Context, OutResult);

	if (!Context.SourceCharacter)
	{
		return;
	}

	if (!Context.GridManager)
	{
		return;
	}

	if (!Context.HasHoveredCoord())
	{
		return;
	}

	if (!Context.GridManager->IsValidGridCoord(Context.HoveredCoord))
	{
		return;
	}

	if (!IsTargetingDataValid(TargetingData))
	{
		return;
	}

	const FMuksiTileTargetingData* Data = TargetingData.GetPtr<FMuksiTileTargetingData>();

	if (!Data)
	{
		return;
	}

	const FIntPoint SourceCoord = Context.SourceCharacter->GetCharacterPosition();
	const int32 Distance = GetHexDistance(Context.GridManager, SourceCoord, Context.HoveredCoord);

	if (Distance > Data->SelectionRange)
	{
		return;
	}

	OutResult.bCanConfirm = true;
	OutResult.SelectedCoord = Context.HoveredCoord;
	OutResult.TargetCharacters = Context.CandidateCharacters;
}

const UScriptStruct* UMuksiTileTargetingPolicy::GetTargetingDataStruct() const
{
	return FMuksiTileTargetingData::StaticStruct();
}

int32 UMuksiTileTargetingPolicy::GetHexDistance(const ABattleGridManager* GridManager, const FIntPoint& StartCoord, const FIntPoint& EndCoord) const
{
	if (!GridManager)
	{
		return MAX_int32;
	}

	const FCubeCoord StartCube = GridManager->OddQToCube(StartCoord);
	const FCubeCoord EndCube = GridManager->OddQToCube(EndCoord);
	const int32 DeltaX = FMath::Abs(StartCube.X - EndCube.X);
	const int32 DeltaY = FMath::Abs(StartCube.Y - EndCube.Y);
	const int32 DeltaZ = FMath::Abs(StartCube.Z - EndCube.Z);

	return FMath::Max3(DeltaX, DeltaY, DeltaZ);
}