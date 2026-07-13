#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Policy/MuksiCardTargetingPolicy.h"

UMuksiBattleCardDataAsset::UMuksiBattleCardDataAsset()
{
	CardID = NAME_None;
	CardName = FText::FromString(TEXT("Default Card"));
	CardDescription = FText::FromString(TEXT("Card Description"));
	CardTexture = nullptr;
}

#if WITH_EDITOR

void UMuksiBattleCardDataAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	SyncExecutionDataTypes();
	SyncTargetingDataType();
	SyncAreaPatternDataType();
}

void UMuksiBattleCardDataAsset::SyncExecutionDataTypes()
{
	for (FMuksiBattleExecutionEntry& Entry : ExecutionChain)
	{
		Entry.SyncExecutionDataType();
	}

	for (FMuksiBattleNotifyExecutionBinding& Binding : NotifyExecutionBindings)
	{
		Binding.SyncExecutionDataTypes();
	}
}

void UMuksiBattleCardDataAsset::SyncTargetingDataType()
{
	if (!TargetingPolicyClass)
	{
		TargetingData.Reset();
		return;
	}

	const UMuksiCardTargetingPolicy* TargetingPolicyCDO = TargetingPolicyClass.GetDefaultObject();

	if (!TargetingPolicyCDO)
	{
		TargetingData.Reset();
		return;
	}

	const UScriptStruct* ExpectedStruct = TargetingPolicyCDO->GetTargetingDataStruct();

	if (!ExpectedStruct)
	{
		TargetingData.Reset();
		return;
	}

	if (TargetingData.GetScriptStruct() == ExpectedStruct)
	{
		return;
	}

	TargetingData.InitializeAs(ExpectedStruct);
}

void UMuksiBattleCardDataAsset::SyncAreaPatternDataType()
{
	if (!AreaPatternClass)
	{
		AreaPatternData.Reset();
		return;
	}

	const UMuksiCardAreaPattern* AreaPatternCDO = AreaPatternClass.GetDefaultObject();

	if (!AreaPatternCDO)
	{
		AreaPatternData.Reset();
		return;
	}

	const UScriptStruct* ExpectedStruct = AreaPatternCDO->GetAreaPatternDataStruct();

	if (!ExpectedStruct)
	{
		AreaPatternData.Reset();
		return;
	}

	if (AreaPatternData.GetScriptStruct() == ExpectedStruct)
	{
		return;
	}

	AreaPatternData.InitializeAs(ExpectedStruct);
}

#endif