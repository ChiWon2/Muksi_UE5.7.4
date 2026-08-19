#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"

UMuksiBattleCardDataAsset::UMuksiBattleCardDataAsset()
{
	CardID = NAME_None;
	CardName = FText::FromString(TEXT("Default Card"));
	CardDescription = FText::FromString(TEXT("Card Description"));
	CardTexture = nullptr;
}

UMuksiBattleCardDataAsset* UMuksiBattleCardDataAsset::GetDeceivedCard() const
{
	if (!bIsDeceiveCard || !IsValid(DeceivedCard) || DeceivedCard.Get() == this)
	{
		return nullptr;
	}

	return DeceivedCard.Get();
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
	for (FBattleExecutionEntry& Entry : MainExecutions)
	{
		Entry.SyncExecutionDataType();
	}

	for (FBattleNotifyExecutionChain& NotifyChain : NotifyExecutionChains)
	{
		NotifyChain.SyncExecutionDataTypes();
	}
}

void UMuksiBattleCardDataAsset::SyncTargetingDataTypes()
{
	TargetingData.SyncDataTypes();
}





#endif
