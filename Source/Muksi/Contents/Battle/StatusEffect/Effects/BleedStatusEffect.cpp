#include "Muksi/Contents/Battle/StatusEffect/Effects/BleedStatusEffect.h"

#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

UBleedStatusEffect::UBleedStatusEffect()
{
	/*FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	DamageData.DefensePolicy = EDamageDefensePolicy::IgnoreDefense;
	DamageData.FloatingTextStyleKey = TEXT("Bleed");
	DamageData.bTriggerHitReaction = false;
	DamageData.bTriggerStatusEffectReactions = false;

	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	ModifyExecutionEntries.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.PlayRate = 1.25f;
	HitReactionData.FXDataAssetKey = TEXT("StatusEffect_OnAffect_Bleed");
	HitReactionData.bWaitForFX = true;

	HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
	ModifyExecutionEntries.Add(MoveTemp(HitReactionEntry));

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;

	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	ModifyExecutionEntries.Add(MoveTemp(RemoveEntry));*/
	
	
	// 공격 스킬을 사용하는 본인에게 출혈 피해
	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope =
		EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy =
		EBattleExecutionTargetPolicy::Attacker;
	DamageData.DefensePolicy =
		EDamageDefensePolicy::IgnoreDefense;
	DamageData.FloatingTextStyleKey = TEXT("Bleed");

	// 피격 연출은 다음 Entry에서 따로 실행
	DamageData.bTriggerHitReaction = false;

	// 출혈 피해로 다른 피격 상태이상 반응을 유발하지 않음
	DamageData.bTriggerStatusEffectReactions = false;

	DamageEntry.ExecutionData
		.InitializeAs<FDamageExecutionData>(DamageData);

	ModifyExecutionEntries.Add(MoveTemp(DamageEntry));

	// 출혈 피격 반응과 이펙트
	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass =
		UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope =
		EBattleExecutionScope::ActualBattleOnly;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.PlayRate = 1.25f;
	HitReactionData.FXDataAssetKey =
		TEXT("StatusEffect_OnAffect_Bleed");
	HitReactionData.bWaitForFX = true;

	HitReactionEntry.ExecutionData
		.InitializeAs<FHitReactionExecutionData>(HitReactionData);

	ModifyExecutionEntries.Add(MoveTemp(HitReactionEntry));

	// 기존 RemoveEntry는 여기서 제거합니다.
}

void UBleedStatusEffect::EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction)
{
	static_cast<void>(OpponentAction);

	// 공격 스킬일 때만 발동
	if (!CurrentAction.Card ||
		CurrentAction.Card->CardTypeInfo.CardType !=
			EMuksiBattleCardType::Attack ||
		GetCurrentStack() <= 0)
	{
		return;
	}

	TArray<FBattleExecutionEntry> BleedEntries =
		ModifyExecutionEntries;

	for (FBattleExecutionEntry& Entry : BleedEntries)
	{
		// 출혈 스택 수를 피해량으로 설정
		if (FDamageExecutionData* DamageData =
			Entry.ExecutionData.GetMutablePtr<FDamageExecutionData>())
		{
			DamageData->DamageValue = GetCurrentStack();
		}

		// 피격 연출도 공격하는 본인에게 실행
		if (Entry.ExecutionData.GetPtr<FHitReactionExecutionData>())
		{
			Entry.ExecutionTargetOverride = CurrentAction.Attacker;
		}
	}

	// 출혈 피해 → 피격 연출 → 원래 스킬 순서
	BleedEntries.Append(CurrentAction.ExecutionEntries);
	CurrentAction.ExecutionEntries = MoveTemp(BleedEntries);
}

void UBleedStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	/*static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart || GetCurrentStack() <= 0)
		return;

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;

	for (FBattleExecutionEntry& Entry : ExecutionEntries)
	{
		if (FDamageExecutionData* DamageData = Entry.ExecutionData.GetMutablePtr<FDamageExecutionData>())
			DamageData->DamageValue = GetCurrentStack();

		if (FStatusEffectExecutionData* StatusEffectData = Entry.ExecutionData.GetMutablePtr<FStatusEffectExecutionData>())
			StatusEffectData->EffectID = GetEffectID();
	}

	OutExecutionEntries.Append(ExecutionEntries);*/
	
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundEnd)
	{
		return;
	}

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass =
		UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope =
		EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.TargetPolicy =
		EBattleExecutionTargetPolicy::ExecutionTarget;
	RemoveData.Operation =
		EStatusEffectExecutionOperation::Remove;
	RemoveData.EffectID = GetEffectID();

	RemoveEntry.ExecutionData
		.InitializeAs<FStatusEffectExecutionData>(RemoveData);

	OutExecutionEntries.Add(MoveTemp(RemoveEntry));
}
