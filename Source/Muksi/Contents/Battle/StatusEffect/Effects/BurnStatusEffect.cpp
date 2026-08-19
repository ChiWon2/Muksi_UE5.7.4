#include "BurnStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void UBurnStatusEffect::BuildPhaseExecutions(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutions)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart)
	{
		return;
	}

	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::SequenceOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.EffectID = GetEffectID();
	SubtractData.Duration = 1;
	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);

	OutExecutions.Add(MoveTemp(SubtractEntry));
}
