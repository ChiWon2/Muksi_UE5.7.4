#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

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
	SyncTargetingDataTypes();
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

void UMuksiBattleCardDataAsset::SyncTargetingDataTypes()
{
	TargetingData.SyncDataTypes();
}

#endif