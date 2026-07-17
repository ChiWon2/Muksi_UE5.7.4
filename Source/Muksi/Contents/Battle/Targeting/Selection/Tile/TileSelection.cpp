#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/BattleGridTile.h"
#include "Muksi/Contents/Battle/Grid/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelectionData.h"

void UTileSelection::Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const
{
	InitializeStepContext(Context, OutStepContext);

	TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(Context, SelectionData);

	const FTileSelectionData* Data = SelectionData.GetPtr<FTileSelectionData>();

	if (!Data)
	{
		return;
	}

	const int32 SafeSelectionRange = FMath::Max(0, Data->SelectionRange);

	for (int32 Y = 0; Y < Context.GridManager->GridHeight; ++Y)
	{
		for (int32 X = 0; X < Context.GridManager->GridWidth; ++X)
		{
			const FIntPoint CandidateCoord(X, Y);

			if (!Context.GridManager->IsValidGridCoord(CandidateCoord))
			{
				continue;
			}

			const int32 HexDistance = FHexGridMath::GetHexDistance(Context.OriginCoord, CandidateCoord);

			if (HexDistance > SafeSelectionRange)
			{
				continue;
			}

			if (!Context.GridManager->GetTileByCoord(CandidateCoord))
			{
				continue;
			}

			OutStepContext.SelectableCoords.Add(CandidateCoord);
		}
	}

	if (!Context.InputContext.HasHoveredCoord())
	{
		return;
	}

	if (!Context.GridManager->IsValidGridCoord(Context.InputContext.HoveredCoord))
	{
		return;
	}

	if (!OutStepContext.IsSelectableCoord(Context.InputContext.HoveredCoord))
	{
		return;
	}

	const ABattleGridTile* SelectedTile = Context.GridManager->GetTileByCoord(Context.InputContext.HoveredCoord);

	if (!SelectedTile)
	{
		return;
	}

	OutStepContext.bCanConfirm = true;
	OutStepContext.SelectedCoord = Context.InputContext.HoveredCoord;
	OutStepContext.SelectedWorldLocation = SelectedTile->GetGridCenterWorldLocation();
	OutStepContext.TargetCharacters = Context.InputContext.CandidateCharacters;
}

const UScriptStruct* UTileSelection::GetSelectionDataStruct() const
{
	return FTileSelectionData::StaticStruct();
}