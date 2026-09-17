// Fill out your copyright notice in the Description page of Project Settings.

#include "Muksi/Contents/Battle/StatusEffect/Effects/SaeMaekStatusEffect.h"

#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void USaeMaekStatusEffect::EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction)
{
	static_cast<void>(OpponentAction);

	if (!CurrentAction.Card || CurrentAction.Card->CardTypeInfo.CardType != EMuksiBattleCardType::Attack || GetCurrentStack() <= 0)
		return;

	TArray<FBattleExecutionEntry>& ExecutionEntries = CurrentAction.ExecutionEntries;

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
	DamageData.DamageValue = GetCurrentStack();
	DamageData.bTriggerHitReaction = false;
	DamageData.bTriggerStatusEffectReactions = true;
	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);

	ExecutionEntries.Insert(MoveTemp(DamageEntry), 0);

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	ExecutionEntries.Insert(MoveTemp(HitReactionEntry), 1);

	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.EffectID = GetEffectID();
	SubtractData.StackCount = 1;
	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);

	ExecutionEntries.Insert(MoveTemp(SubtractEntry), 2);
}
