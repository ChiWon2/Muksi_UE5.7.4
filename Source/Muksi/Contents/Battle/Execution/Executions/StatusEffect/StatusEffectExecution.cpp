#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

void UStatusEffectExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FStatusEffectExecutionData* StatusEffectData = Context.GetExecutionData<FStatusEffectExecutionData>();
	ABattleCharacterBase* TargetCharacter = Context.ExecutionTarget ? Context.ExecutionTarget.Get() : Context.Attacker.Get();
	UMuksiStatusEffectComponent* StatusEffectComponent = TargetCharacter ? TargetCharacter->GetStatusEffectComponent() : nullptr;

	if (!StatusEffectData || StatusEffectData->EffectID.IsNone() || !StatusEffectComponent || Context.ExecutionMode != EBattleExecutionMode::Sequence)
	{
		FinishExecution(OnFinished);
		return;
	}

	switch (StatusEffectData->Operation)
	{
	case EStatusEffectExecutionOperation::Add:
		StatusEffectComponent->AddStatusEffect(StatusEffectData->EffectID, StatusEffectData->StackCount, StatusEffectData->Duration);
		break;
	case EStatusEffectExecutionOperation::Subtract:
		StatusEffectComponent->SubtractStatusEffect(StatusEffectData->EffectID, StatusEffectData->StackCount, StatusEffectData->Duration);
		break;
	case EStatusEffectExecutionOperation::Remove:
		StatusEffectComponent->RemoveStatusEffectByID(StatusEffectData->EffectID);
		break;
	default:
		break;
	}

	FinishExecution(OnFinished);
}

const UScriptStruct* UStatusEffectExecution::GetExecutionDataStruct() const
{
	return FStatusEffectExecutionData::StaticStruct();
}
