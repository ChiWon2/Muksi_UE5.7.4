#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"

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

void UMuksiBattleCardDataAsset::TriggerCardEffects(EBattleCardEffectTrigger InTrigger,
	const FBattleCardEffectContext& Context)
{
	/*for (UBattleCardEffect* Effect : CardEffects)
	{
		if (!IsValid(Effect))
		{
			continue;
		}

		if (Effect->GetTrigger() != InTrigger)
		{
			continue;
		}

		Effect->Execute(Context);
	}*/
}

void UMuksiBattleCardDataAsset::TriggerDamageCardEffects(EBattleCardEffectTrigger InTrigger,
	const FBattleDamageContext& DamageContext)
{
	
}

#endif
