#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Selection/Tile/TileSelectionData.h"

bool UTileSelection::EvaluateCandidate(
	ABattleGridManager* GridManager,
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord& CandidateCoord,
	const FInstancedStruct& SelectionData,
	FTargetingStep& OutStep) const
{
	InitializeStep(OriginCoord, OutStep);

	if (!GridManager || !OriginCoord.IsValid() || !GridManager->IsValidCoord(OriginCoord) || !IsRuleDataValid(SelectionData))
		return false;

	const FTileSelectionData* Data = SelectionData.GetPtr<FTileSelectionData>();
	if (!Data || !CandidateCoord.IsValid() || !GridManager->IsValidCoord(CandidateCoord))
		return false;

	const int32 SafeSelectionRange = FMath::Max(0, Data->SelectionRange);
	if (FHexGridMath::GetHexDistance(OriginCoord, CandidateCoord) > SafeSelectionRange)
		return false;

	OutStep.TargetCoord = CandidateCoord;
	return true;
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
