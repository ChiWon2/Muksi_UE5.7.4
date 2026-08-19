// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/BleedStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void UBleedStatusEffect::BuildPhaseExecutions(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutions)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart || GetCurrentStack() <= 0)
	{
		return;
	}

	FBattleExecutionEntry DamageEntry;
	DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
	DamageEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FDamageExecutionData DamageData;
	DamageData.TargetPolicy = EDamageExecutionTargetPolicy::ExecutionTarget;
	DamageData.DamageValue = GetCurrentStack();
	DamageData.bTriggerHitReaction = true;
	DamageData.bTriggerStatusEffectReactions = false;
	DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
	OutExecutions.Add(MoveTemp(DamageEntry));

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;
	RemoveData.EffectID = GetEffectID();
	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	OutExecutions.Add(MoveTemp(RemoveEntry));
}
