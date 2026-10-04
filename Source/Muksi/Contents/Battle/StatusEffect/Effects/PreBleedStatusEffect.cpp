#include "Muksi/Contents/Battle/StatusEffect/Effects/PreBleedStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

UPreBleedStatusEffect::UPreBleedStatusEffect()
{
	FBattleExecutionEntry AddBleedEntry;
	AddBleedEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	AddBleedEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData AddBleedData;
	AddBleedData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	AddBleedData.Operation = EStatusEffectExecutionOperation::Add;
	AddBleedData.EffectID = MuksiStatusEffectIDs::Bleed;
	AddBleedData.Duration = 1;

	AddBleedEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(AddBleedData);
	ModifyExecutionEntries.Add(MoveTemp(AddBleedEntry));

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;

	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	ModifyExecutionEntries.Add(MoveTemp(RemoveEntry));
}

void UPreBleedStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundEnd || GetCurrentStack() <= 0)
		return;

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;

	if (ExecutionEntries.IsValidIndex(0))
	{
		FStatusEffectExecutionData* AddBleedData = ExecutionEntries[0].ExecutionData.GetMutablePtr<FStatusEffectExecutionData>();
		if (AddBleedData)
			AddBleedData->StackCount = GetCurrentStack();
	}

	if (ExecutionEntries.IsValidIndex(1))
	{
		FStatusEffectExecutionData* RemoveData = ExecutionEntries[1].ExecutionData.GetMutablePtr<FStatusEffectExecutionData>();
		if (RemoveData)
			RemoveData->EffectID = GetEffectID();
	}

	OutExecutionEntries.Append(ExecutionEntries);
}
