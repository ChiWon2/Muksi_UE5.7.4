#include "Muksi/Contents/Battle/Targeting/Policy/MuksiCardTargetingPolicy.h"

void UMuksiCardTargetingPolicy::BuildTargetingResult(const FMuksiCardTargetingContext& Context, const FInstancedStruct& TargetingData, FMuksiCardTargetingResult& OutResult) const
{
	InitializeResult(Context, OutResult);
}

const UScriptStruct* UMuksiCardTargetingPolicy::GetTargetingDataStruct() const
{
	return nullptr;
}

bool UMuksiCardTargetingPolicy::IsTargetingDataValid(const FInstancedStruct& TargetingData) const
{
	const UScriptStruct* ExpectedStruct = GetTargetingDataStruct();

	if (!ExpectedStruct)
	{
		return !TargetingData.IsValid();
	}

	return TargetingData.GetScriptStruct() == ExpectedStruct;
}

void UMuksiCardTargetingPolicy::InitializeResult(const FMuksiCardTargetingContext& Context, FMuksiCardTargetingResult& OutResult) const
{
	OutResult.Reset();
	OutResult.AimWorldLocation = Context.AimWorldLocation;
}