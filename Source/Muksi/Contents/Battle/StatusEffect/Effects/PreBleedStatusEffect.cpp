// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/PreBleedStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

void UPreBleedStatusEffect::BuildPhaseExecutions(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutions)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundEnd || GetCurrentStack() <= 0)
	{
		return;
	}

	FBattleExecutionEntry AddBleedEntry;
	AddBleedEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	AddBleedEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData AddBleedData;
	AddBleedData.Operation = EStatusEffectExecutionOperation::Add;
	AddBleedData.EffectID = MuksiStatusEffectIDs::Bleed;
	AddBleedData.StackCount = GetCurrentStack();
	AddBleedData.Duration = 1;
	AddBleedEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(AddBleedData);
	OutExecutions.Add(MoveTemp(AddBleedEntry));

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;
	RemoveData.EffectID = GetEffectID();
	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	OutExecutions.Add(MoveTemp(RemoveEntry));
}
