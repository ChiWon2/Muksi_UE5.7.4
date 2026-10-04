#include "Muksi/Contents/Battle/StatusEffect/Effects/BurnStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

UBurnStatusEffect::UBurnStatusEffect()
{
	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.Duration = 1;

	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);
	ModifyExecutionEntries.Add(MoveTemp(SubtractEntry));
}

void UBurnStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart)
		return;

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;

	for (FBattleExecutionEntry& Entry : ExecutionEntries)
	{
		if (FStatusEffectExecutionData* StatusEffectData = Entry.ExecutionData.GetMutablePtr<FStatusEffectExecutionData>())
			StatusEffectData->EffectID = GetEffectID();
	}

	OutExecutionEntries.Append(ExecutionEntries);
}
