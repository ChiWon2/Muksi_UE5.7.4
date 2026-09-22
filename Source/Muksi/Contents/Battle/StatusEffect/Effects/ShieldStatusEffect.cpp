#include "Muksi/Contents/Battle/StatusEffect/Effects/ShieldStatusEffect.h"

#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"

int32 UShieldStatusEffect::GetIncomingDamageModifierPriority() const
{
	return 200;
}

void UShieldStatusEffect::ModifyIncomingDamage(FIncomingDamageModifierContext& Context)
{
	if (Context.RemainingDamage <= 0 || GetCurrentStack() <= 0)
		return;

	const int32 AbsorbedDamage = FMath::Min(Context.RemainingDamage, GetCurrentStack());
	ConsumeStack(AbsorbedDamage);
	Context.RemainingDamage -= AbsorbedDamage;
}

FName UShieldStatusEffect::GetHitReactionAnimKey() const
{
	return TEXT("Block_Hit");
}

void UShieldStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	static_cast<void>(OldPhase);

	if (!bExpireAtRoundEnd || NewPhase != EBattlePhase::RoundEnd)
		return;

	FBattleExecutionEntry RemoveEntry;
	RemoveEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
	RemoveEntry.ExecutionScope = EBattleExecutionScope::ActualBattleOnly;

	FStatusEffectExecutionData RemoveData;
	RemoveData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
	RemoveData.Operation = EStatusEffectExecutionOperation::Remove;
	RemoveData.EffectID = GetEffectID();
	RemoveEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(RemoveData);
	OutExecutionEntries.Add(MoveTemp(RemoveEntry));
}
