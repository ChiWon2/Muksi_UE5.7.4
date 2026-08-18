#include "ParalysisStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void UParalysisStatusEffect::BuildPhaseExecutions(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutions)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::RoundEnd)
	{
		return;
	}

	FBattleExecutionEntry StatusEntry;
	StatusEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	StatusEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData StatusData;
	StatusData.Operation = NewPhase == EBattlePhase::RoundStart ? EStatusEffectExecutionOperation::Subtract : EStatusEffectExecutionOperation::Remove;
	StatusData.EffectID = GetEffectID();
	StatusData.Duration = NewPhase == EBattlePhase::RoundStart ? 1 : 0;
	StatusEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(StatusData);

	OutExecutions.Add(MoveTemp(StatusEntry));
}
