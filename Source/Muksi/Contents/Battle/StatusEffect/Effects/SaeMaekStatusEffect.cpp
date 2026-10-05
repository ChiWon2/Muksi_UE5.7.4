#include "Muksi/Contents/Battle/StatusEffect/Effects/SaeMaekStatusEffect.h"

#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

USaeMaekStatusEffect::USaeMaekStatusEffect()
{
	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
	DamageData.DefensePolicy = EDamageDefensePolicy::IgnoreDefense;
	DamageData.bTriggerHitReaction = false;
	DamageData.bTriggerStatusEffectReactions = true;

	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	ModifyExecutionEntries.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.PlayRate = 1.25f;
	HitReactionData.FXDataAssetKey = TEXT("StatusEffect_OnAffect_Saemaek");
	HitReactionData.bWaitForFX = true;

	HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
	ModifyExecutionEntries.Add(MoveTemp(HitReactionEntry));

	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.StackCount = 1;

	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);
	ModifyExecutionEntries.Add(MoveTemp(SubtractEntry));
}

void USaeMaekStatusEffect::EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction)
{
	static_cast<void>(OpponentAction);

	if (!CurrentAction.Card || CurrentAction.Card->CardTypeInfo.CardType != EMuksiBattleCardType::Attack || GetCurrentStack() <= 0)
		return;

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;

	for (FBattleExecutionEntry& Entry : ExecutionEntries)
	{
		if (FDamageExecutionData* DamageData = Entry.ExecutionData.GetMutablePtr<FDamageExecutionData>())
			DamageData->DamageValue = GetCurrentStack();

		if (Entry.ExecutionData.GetPtr<FHitReactionExecutionData>())
			Entry.ExecutionTargetOverride = CurrentAction.Attacker;

		if (FStatusEffectExecutionData* StatusEffectData = Entry.ExecutionData.GetMutablePtr<FStatusEffectExecutionData>())
			StatusEffectData->EffectID = GetEffectID();
	}

	ExecutionEntries.Append(CurrentAction.ExecutionEntries);
	CurrentAction.ExecutionEntries = MoveTemp(ExecutionEntries);
}
