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

void USaeMaekStatusEffect::BuildBattleActionStartExecutions(const FBattleAction& BattleAction, TArray<FBattleExecutionEntry>& OutExecutions)
{
	if (!BattleAction.Card || BattleAction.Card->CardTypeInfo.CardType != EMuksiBattleCardType::Attack || GetCurrentStack() <= 0)
	{
		return;
	}

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EDamageExecutionTargetPolicy::Attacker;
	DamageData.DamageValue = GetCurrentStack();
	DamageData.bTriggerHitReaction = false;
	DamageData.bTriggerStatusEffectReactions = true;
	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);

	OutExecutions.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry HitReactionEntry;
	HitReactionEntry.ExecutionClass = UHitReactionExecution::StaticClass();
	HitReactionEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	OutExecutions.Add(MoveTemp(HitReactionEntry));

	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.EffectID = GetEffectID();
	SubtractData.StackCount = 1;
	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);

	OutExecutions.Add(MoveTemp(SubtractEntry));
}
