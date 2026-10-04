#include "Muksi/Contents/Battle/StatusEffect/Effects/ParalysisStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

UParalysisStatusEffect::UParalysisStatusEffect()
{
	FBattleExecutionEntry StatusEntry;
	StatusEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	StatusEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData StatusData;
	StatusData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;

	StatusEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(StatusData);
	ModifyExecutionEntries.Add(MoveTemp(StatusEntry));
}

void UParalysisStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::RoundEnd)
		return;

	TArray<FBattleExecutionEntry> ExecutionEntries = ModifyExecutionEntries;

	for (FBattleExecutionEntry& Entry : ExecutionEntries)
	{
		FStatusEffectExecutionData* StatusData = Entry.ExecutionData.GetMutablePtr<FStatusEffectExecutionData>();
		if (!StatusData)
			continue;

		StatusData->Operation = NewPhase == EBattlePhase::RoundStart ? EStatusEffectExecutionOperation::Subtract : EStatusEffectExecutionOperation::Remove;
		StatusData->EffectID = GetEffectID();
		StatusData->Duration = NewPhase == EBattlePhase::RoundStart ? 1 : 0;
	}

	OutExecutionEntries.Append(ExecutionEntries);
}
