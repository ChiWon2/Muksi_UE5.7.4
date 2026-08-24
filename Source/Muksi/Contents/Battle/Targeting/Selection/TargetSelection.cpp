#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

void UTargetSelection::EvaluateCandidate(
	ABattleGridManager*,
	EBattleSimulationWorldType,
	const FHexOffsetCoord& OriginCoord,
	const FHexOffsetCoord&,
	const FInstancedStruct&,
	FTargetingStepResult& OutStepResult) const
{
	InitializeStepResult(OriginCoord, OutStepResult);
}

const UScriptStruct* UTargetSelection::GetSelectionDataStruct() const
{
	return nullptr;
}

bool UTargetSelection::IsSelectionDataValid(const FInstancedStruct& SelectionData) const
{
	const UScriptStruct* ExpectedStruct = GetSelectionDataStruct();

	if (!ExpectedStruct)
	{
		return !SelectionData.IsValid();
	}

	return SelectionData.GetScriptStruct() == ExpectedStruct;
}

void UTargetSelection::InitializeStepResult(const FHexOffsetCoord& OriginCoord, FTargetingStepResult& OutStepResult) const
{
	OutStepResult.Reset();
	OutStepResult.OriginCoord = OriginCoord;
}
