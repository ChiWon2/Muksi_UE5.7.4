#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

void UStatusEffectExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FStatusEffectExecutionData* StatusEffectData = Context.GetExecutionData<FStatusEffectExecutionData>();

	if (!StatusEffectData || StatusEffectData->EffectID.IsNone() || Context.ExecutionMode != EBattleExecutionMode::ActualBattle)
	{
		FinishExecution(OnFinished);
		return;
	}

	TArray<ABattleCharacterBase*> TargetCharacters;
	CollectTargets(Context, *StatusEffectData, TargetCharacters);

	for (ABattleCharacterBase* TargetCharacter : TargetCharacters)
		ApplyStatusEffectToTarget(*StatusEffectData, TargetCharacter);

	FinishExecution(OnFinished);
}

void UStatusEffectExecution::CollectTargets(const FBattleExecutionContext& Context, const FStatusEffectExecutionData& StatusEffectData, TArray<ABattleCharacterBase*>& OutTargets) const
{
	OutTargets.Reset();

	switch (StatusEffectData.TargetPolicy)
	{
	case EBattleExecutionTargetPolicy::ExecutionTarget:
		if (Context.ExecutionTarget)
			OutTargets.Add(Context.ExecutionTarget);
		return;
	case EBattleExecutionTargetPolicy::Attacker:
		if (Context.Attacker)
			OutTargets.Add(Context.Attacker);
		return;
	case EBattleExecutionTargetPolicy::TargetingResult:
	default:
		break;
	}

	const FTargetingStepResult* StepResult = Context.GetLastTargetingStepResult();

	if (!StepResult)
		return;

	for (ABattleCharacterBase* TargetCharacter : StepResult->Targets)
	{
		if (TargetCharacter)
			OutTargets.AddUnique(TargetCharacter);
	}
}

void UStatusEffectExecution::ApplyStatusEffectToTarget(const FStatusEffectExecutionData& StatusEffectData, ABattleCharacterBase* TargetCharacter) const
{
	if (!TargetCharacter)
		return;

	UMuksiStatusEffectComponent* StatusEffectComponent = TargetCharacter->GetStatusEffectComponent();

	if (!StatusEffectComponent)
		return;

	switch (StatusEffectData.Operation)
	{
	case EStatusEffectExecutionOperation::Add:
		StatusEffectComponent->AddStatusEffect(StatusEffectData.EffectID, StatusEffectData.StackCount, StatusEffectData.Duration);
		break;
	case EStatusEffectExecutionOperation::Subtract:
		StatusEffectComponent->SubtractStatusEffect(StatusEffectData.EffectID, StatusEffectData.StackCount, StatusEffectData.Duration);
		break;
	case EStatusEffectExecutionOperation::Remove:
		StatusEffectComponent->RemoveStatusEffectByID(StatusEffectData.EffectID);
		break;
	default:
		break;
	}
}

const UScriptStruct* UStatusEffectExecution::GetExecutionDataStruct() const
{
	return FStatusEffectExecutionData::StaticStruct();
}
