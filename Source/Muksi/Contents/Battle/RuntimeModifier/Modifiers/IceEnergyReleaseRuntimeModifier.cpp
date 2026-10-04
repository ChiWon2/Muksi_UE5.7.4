#include "Muksi/Contents/Battle/RuntimeModifier/Modifiers/IceEnergyReleaseRuntimeModifier.h"

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

namespace IceEnergyReleaseRuntimeModifier
{
	constexpr int32 MaxAdditionalAttackCount = 3;
	const FName HitFXDataAssetKey = TEXT("HitReaction_Ice");
	const FName FinalHitFXDataAssetKey = TEXT("Impact_Ice");

	ABattleCharacterBase* FindTargetCharacter(const FBattleAction& Action)
	{
		for (const FTargetingStepResult& StepResult : Action.TargetingResult.Steps)
		{
			for (ABattleCharacterBase* TargetCharacter : StepResult.GetAllTargets())
			{
				if (IsValid(TargetCharacter) && TargetCharacter != Action.Attacker.Get())
					return TargetCharacter;
			}
		}

		return nullptr;
	}
}

UIceEnergyReleaseRuntimeModifier::UIceEnergyReleaseRuntimeModifier()
{
	const FName ComboAnimKeys[IceEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount]
	{
		TEXT("Combo_1"),
		TEXT("Combo_2"),
		TEXT("Combo_3")
	};

	const int32 ComboDamageValues[IceEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount]
	{
		8,
		15,
		30
	};

	for (int32 AttackIndex = 0; AttackIndex < IceEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount; ++AttackIndex)
	{
		FBattleExecutionEntry ComboEntry;
		ComboEntry.ExecutionClass = UPlayMontageExecution::StaticClass();
		ComboEntry.ExecutionScope = EBattleExecutionScope::Both;

		FPlayMontageExecutionData ComboData;
		ComboData.AnimKey = ComboAnimKeys[AttackIndex];
		ComboData.PlayRate = 1.35f;

		ComboEntry.ExecutionData.InitializeAs<FPlayMontageExecutionData>(ComboData);
		ModifierExecutionEntries.Add(MoveTemp(ComboEntry));

		FBattleExecutionNotify MainEffectNotify;
		MainEffectNotify.NotifyKey = TEXT("ComboEffect");
		MainEffectNotify.SourceAnimKey = ComboAnimKeys[AttackIndex];

		FBattleExecutionEntry ConsumeEntry;
		ConsumeEntry.ExecutionClass = UStatusEffectExecution::StaticClass();
		ConsumeEntry.ExecutionScope = EBattleExecutionScope::Both;

		FStatusEffectExecutionData ConsumeData;
		ConsumeData.TargetPolicy = EBattleExecutionTargetPolicy::Attacker;
		ConsumeData.Operation = EStatusEffectExecutionOperation::Subtract;
		ConsumeData.EffectID = MuksiStatusEffectIDs::IceEnergy;
		ConsumeData.StackCount = 1;

		ConsumeEntry.ExecutionData.InitializeAs<FStatusEffectExecutionData>(ConsumeData);
		MainEffectNotify.ExecutionEntries.Add(MoveTemp(ConsumeEntry));

		FBattleExecutionEntry DamageEntry;
		DamageEntry.ExecutionClass = UDamageExecution::StaticClass();
		DamageEntry.ExecutionScope = EBattleExecutionScope::Both;

		FDamageExecutionData DamageData;
		DamageData.TargetPolicy = EBattleExecutionTargetPolicy::ExecutionTarget;
		DamageData.DamageValue = ComboDamageValues[AttackIndex];
		DamageData.HitFXDataAssetKey = IceEnergyReleaseRuntimeModifier::HitFXDataAssetKey;

		DamageEntry.ExecutionData.InitializeAs<FDamageExecutionData>(DamageData);
		MainEffectNotify.ExecutionEntries.Add(MoveTemp(DamageEntry));

		ModifierExecutionNotifies.Add(MoveTemp(MainEffectNotify));
	}
}

void UIceEnergyReleaseRuntimeModifier::ModifyBattleAction(FBattleAction& Action) const
{
	ABattleCharacterBase* Attacker = Action.Attacker.Get();

	if (!IsValid(Attacker))
		return;

	UMuksiStatusEffectComponent* StatusEffectComponent = Attacker->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
		return;

	const int32 IceEnergyStack = StatusEffectComponent->GetEffectStackCount(MuksiStatusEffectIDs::IceEnergy);
	const int32 AdditionalAttackCount = FMath::Min(IceEnergyStack, IceEnergyReleaseRuntimeModifier::MaxAdditionalAttackCount);

	if (AdditionalAttackCount <= 0)
		return;

	ABattleCharacterBase* TargetCharacter = IceEnergyReleaseRuntimeModifier::FindTargetCharacter(Action);

	for (int32 AttackIndex = 0; AttackIndex < AdditionalAttackCount; ++AttackIndex)
	{
		if (!ModifierExecutionEntries.IsValidIndex(AttackIndex) || !ModifierExecutionNotifies.IsValidIndex(AttackIndex))
			break;

		FBattleExecutionEntry ComboEntry = ModifierExecutionEntries[AttackIndex];
		ComboEntry.ExecutionSourceOverride = Attacker;
		ComboEntry.ExecutionTargetOverride = TargetCharacter;
		Action.ExecutionEntries.Add(MoveTemp(ComboEntry));

		FBattleExecutionNotify MainEffectNotify = ModifierExecutionNotifies[AttackIndex];
		MainEffectNotify.NotifySourceOverride = Attacker;

		for (FBattleExecutionEntry& NotifyEntry : MainEffectNotify.ExecutionEntries)
		{
			NotifyEntry.ExecutionSourceOverride = Attacker;
			NotifyEntry.ExecutionTargetOverride = TargetCharacter;

			FDamageExecutionData* DamageData = NotifyEntry.ExecutionData.GetMutablePtr<FDamageExecutionData>();
			if (!DamageData)
				continue;

			DamageData->TargetPolicy = IsValid(TargetCharacter) ? EBattleExecutionTargetPolicy::ExecutionTarget : EBattleExecutionTargetPolicy::TargetingResult;

			DamageData->HitFXDataAssetKey = AttackIndex == AdditionalAttackCount - 1 ? IceEnergyReleaseRuntimeModifier::FinalHitFXDataAssetKey : IceEnergyReleaseRuntimeModifier::HitFXDataAssetKey;
		}

		Action.ExecutionNotifies.Add(MoveTemp(MainEffectNotify));
	}
}
