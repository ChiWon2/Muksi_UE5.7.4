// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/BleedStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void UBleedStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart || GetCurrentStack() <= 0)
		return;

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	DamageData.DamageValue = GetCurrentStack();
	DamageData.DefensePolicy = EDamageDefensePolicy::IgnoreDefense;
	DamageData.bTriggerHitReaction = false;
	DamageData.bTriggerStatusEffectReactions = false;
	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	OutExecutionEntries.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FHitReactionExecutionData HitReactionData;
	HitReactionData.PlayRate = 1.25f;
	HitReactionData.FXDataAssetKey = TEXT("StatusEffect_OnAffect_Bleed");
	HitReactionData.bWaitForFX = true;
	HitReactionEntry.ExecutionData.InitializeAs<FHitReactionExecutionData>(HitReactionData);
	OutExecutionEntries.Add(MoveTemp(HitReactionEntry));

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;
	RemoveData.EffectID = GetEffectID();
	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	OutExecutionEntries.Add(MoveTemp(RemoveEntry));
}
