#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"

bool UTargetSelection::EvaluateCandidate(
	ABattleGridManager*,
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord&,
	const FInstancedStruct&,
	FTargetingStep& OutStep) const
{
	InitializeStep(OriginCoord, OutStep);
	return false;
}

void UTargetSelection::CollectCandidateCoords(
    ABattleGridManager* GridManager,
    const FHexOffsetCoord& OriginCoord,
    const FInstancedStruct& SelectionData,
    TArray<FHexOffsetCoord>& OutCoords) const
{
    OutCoords.Reset();

    if (!GridManager || !OriginCoord.IsValid() || !IsRuleDataValid(SelectionData))
        return;

    for (int32 X = 0; X < GridManager->GetGridWidth(); ++X)
    {
        for (int32 Y = 0; Y < GridManager->GetGridHeight(); ++Y)
            OutCoords.Add(FHexOffsetCoord(X, Y));
    }
}

const UScriptStruct* UTargetSelection::GetRuleDataStruct() const
{
	return nullptr;
}

bool UTargetSelection::IsRuleDataValid(const FInstancedStruct& SelectionData) const
{
	const UScriptStruct* ExpectedStruct = GetRuleDataStruct();

	if (!ExpectedStruct)
	{
		return !SelectionData.IsValid();
	}

	return SelectionData.GetScriptStruct() == ExpectedStruct;
}

void UTargetSelection::InitializeStep(const FHexOffsetCoord& OriginCoord, FTargetingStep& OutStep) const
{
	OutStep.Reset();
	OutStep.OriginCoord = OriginCoord;
}
