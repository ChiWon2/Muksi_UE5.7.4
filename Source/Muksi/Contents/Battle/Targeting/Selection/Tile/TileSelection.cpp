#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelectionData.h"

void UTileSelection::EvaluateCandidate(
	ABattleGridManager* GridManager,
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& CandidateCoord,
	const FInstancedStruct& SelectionData,
	FSelectionStepResult& OutStepResult) const
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

	OutStepResult.bValid = true;
	OutStepResult.SelectedCoord = CandidateCoord;
}

void UTileSelection::CollectCandidateCoords(
    ABattleGridManager* GridManager,
    const FHexOffsetCoord& OriginCoord,
    const FInstancedStruct& SelectionData,
    TArray<FHexOffsetCoord>& OutCoords) const
{
    OutCoords.Reset();

    TARGET_SELECTION_VALIDATE_COMMON_OR_RETURN(GridManager, OriginCoord, SelectionData);

    const FTileSelectionData* Data = SelectionData.GetPtr<FTileSelectionData>();

    if (!Data)
        return;

    const int32 SelectionRange = FMath::Max(0, Data->SelectionRange);

    for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
    {
        for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
        {
            const FHexOffsetCoord CandidateCoord(X, Y);

            if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) <= SelectionRange)
                OutCoords.Add(CandidateCoord);
        }
    }
}

const UScriptStruct* UTileSelection::GetRuleDataStruct() const
{
	return FTileSelectionData::StaticStruct();
}
