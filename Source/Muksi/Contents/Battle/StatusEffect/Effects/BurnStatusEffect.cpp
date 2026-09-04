#include "BurnStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

void UBurnStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (NewPhase != EBattlePhase::RoundStart)
	{
		return;
	}

	FBattleExecutionEntry SubtractEntry;
	SubtractEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	SubtractEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData SubtractData;
	SubtractData.Operation = EStatusEffectExecutionOperation::Subtract;
	SubtractData.EffectID = GetEffectID();
	SubtractData.Duration = 1;
	SubtractEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(SubtractData);

	OutExecutionEntries.Add(MoveTemp(SubtractEntry));
}
