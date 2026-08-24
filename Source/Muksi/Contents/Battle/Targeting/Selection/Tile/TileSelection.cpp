#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelectionData.h"

void UTileSelection::EvaluateCandidate(
	ABattleGridManager* GridManager,
	EBattleSimulationWorldType,
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& CandidateCoord,
	const FInstancedStruct& SelectionData,
	FTargetingStepResult& OutStepResult) const
{
	InitializeStepResult(OriginCoord, OutStepResult);

	TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(GridManager, OriginCoord, SelectionData);

	const FTileSelectionData* Data = SelectionData.GetPtr<FTileSelectionData>();

	if (!Data)
	{
		return;
	}

	const int32 SafeSelectionRange = FMath::Max(0, Data->SelectionRange);

	TARGET_SELECTION_VALIDATE_CANDIDATE_OR_RETURN(GridManager, CandidateCoord);

	if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) > SafeSelectionRange)
	{
		return;
	}

	const ABattleGridTile* SelectedTile = GridManager->GetTileActorByCoord(CandidateCoord);

	if (!SelectedTile)
	{
		return;
	}

	OutStepResult.bValid = true;
	OutStepResult.SelectedCoord = CandidateCoord;
}

const UScriptStruct* UTileSelection::GetSelectionDataStruct() const
{
	return FTileSelectionData::StaticStruct();
}
