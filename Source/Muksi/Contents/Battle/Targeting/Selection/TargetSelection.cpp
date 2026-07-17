#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"

void UTargetSelection::Evaluate(const FTargetSelectionContext& Context, const FInstancedStruct& SelectionData, FTargetingStepContext& OutStepContext) const
{
	InitializeStepContext(Context, OutStepContext);
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

void UTargetSelection::InitializeStepContext(const FTargetSelectionContext& Context, FTargetingStepContext& OutStepContext) const
{
	OutStepContext.Reset();
	OutStepContext.OriginCoord = Context.OriginCoord;
	OutStepContext.OriginWorldLocation = Context.OriginWorldLocation;
	OutStepContext.AimWorldLocation = Context.InputContext.AimWorldLocation;
}