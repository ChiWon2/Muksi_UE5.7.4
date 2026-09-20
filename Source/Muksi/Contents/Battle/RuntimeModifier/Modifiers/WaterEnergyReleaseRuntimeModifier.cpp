#include "Muksi/Contents/Battle/RuntimeModifier/Modifiers/WaterEnergyReleaseRuntimeModifier.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/DamageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/PlayMontage/PlayMontageExecutionData.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecution.h"
#include "Muksi/Contents/Battle/Execution/Executions/StatusEffect/StatusEffectExecutionData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

namespace WaterEnergyReleaseRuntimeModifier
{
	constexpr int32 MaxAdditionalAttackCount = 3;

	const FName ComboAnimKeys[MaxAdditionalAttackCount]
	{
		TEXT("Combo_1"),
		TEXT("Combo_2"),
		TEXT("Combo_3")
	};

	const FName MainEffectNotifyKey = TEXT("ComboEffect");

	ABattleCharacterBase* FindTargetCharacter(const FBattleAction& Action)
	{
		for (const FTargetingStepResult& StepResult : Action.TargetingResult.Steps)
		{
			for (ABattleCharacterBase* TargetCharacter : StepResult.Targets)
			{
				if (IsValid(TargetCharacter) && TargetCharacter != Action.Attacker.Get())
					return TargetCharacter;
			}
		}

		return nullptr;
	}

	FBattleExecutionEntry MakeConsumeWaterEnergyEntry(ABattleCharacterBase* Attacker)
	{
		FBattleExecutionEntry ConsumeEntry;
		ConsumeEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
		ConsumeEntry.ExecutionScope = EBattleExecutionScope::Both;
		ConsumeEntry.ExecutionSourceOverride = Attacker;

		FStatusEffectExecutionData ConsumeData;
		ConsumeData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
		ConsumeData.Operation = EStatusEffectExecutionOperation::Subtract;
		ConsumeData.EffectID = MuksiStatusEffectIDs::WaterEnergy;
		ConsumeData.StackCount = 1;

		ConsumeEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(ConsumeData);
		return ConsumeEntry;
	}

	FBattleExecutionEntry MakeDamageEntry(int32 DamageValue, ABattleCharacterBase* Attacker, ABattleCharacterBase* TargetCharacter)
	{
		FBattleExecutionEntry DamageEntry;
		DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
		DamageEntry.ExecutionScope = EBattleExecutionScope::Both;
		DamageEntry.ExecutionSourceOverride = Attacker;
		DamageEntry.ExecutionTargetOverride = TargetCharacter;

		FDamageExecutionData DamageData;
		DamageData.TargetPolicy = IsValid(TargetCharacter)
			? EBattleExecutionTargetPolicy::ExecutionTarget
			: EBattleExecutionTargetPolicy::TargetingResult;
		DamageData.DamageValue = DamageValue;

		DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
		return DamageEntry;
	}
}

void UWaterEnergyReleaseRuntimeModifier::ModifyBattleAction(FBattleAction& Action) const
{
	ABattleCharacterBase* Attacker = Action.Attacker.Get();

	if (!IsValid(Attacker))
		return;

	UMuksiStatusEffectComponent* StatusEffectComponent = Attacker->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
		return;

	const int32 WaterEnergyStack = StatusEffectComponent->GetEffectStackCount(MuksiStatusEffectIDs::WaterEnergy);
	const int32 AdditionalAttackCount = FMath::Min(WaterEnergyStack, WaterEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount);

	if (AdditionalAttackCount <= 0)
		return;

	ABattleCharacterBase* TargetCharacter = WaterEnergyReleaseRuntimeModifier::FindTargetCharacter(Action);
	const int32 ComboDamageValues[WaterEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount]
	{
		Combo1Damage,
		Combo2Damage,
		Combo3Damage
	};

	for (int32 AttackIndex = 0; AttackIndex < AdditionalAttackCount; ++AttackIndex)
	{
		const FName ComboAnimKey = WaterEnergyReleaseRuntimeModifier::ComboAnimKeys[AttackIndex];

		FBattleExecutionEntry ComboEntry;
		ComboEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
		ComboEntry.ExecutionScope = EBattleExecutionScope::Both;
		ComboEntry.ExecutionSourceOverride = Attacker;
		ComboEntry.ExecutionTargetOverride = TargetCharacter;

		FPlayMontageExecutionData ComboData;
		ComboData.AnimKey = ComboAnimKey;
		ComboData.PlayRate = ComboPlayRate;

		ComboEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(ComboData);
		Action.ExecutionEntries.Add(MoveTemp(ComboEntry));

		FBattleExecutionNotify MainEffectNotify;
		MainEffectNotify.NotifyKey = WaterEnergyReleaseRuntimeModifier::MainEffectNotifyKey;
		MainEffectNotify.SourceAnimKey = ComboAnimKey;
		MainEffectNotify.NotifySourceOverride = Attacker;
		MainEffectNotify.ExecutionEntries.Add(
			WaterEnergyReleaseRuntimeModifier::MakeConsumeWaterEnergyEntry(Attacker));
		MainEffectNotify.ExecutionEntries.Add(
			WaterEnergyReleaseRuntimeModifier::MakeDamageEntry(
				ComboDamageValues[AttackIndex],
				Attacker,
				TargetCharacter));

		Action.ExecutionNotifies.Add(MoveTemp(MainEffectNotify));
	}
}
