#include "ChargeWaterEnergyStatusEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

void UChargeWaterEnergyStatusEffect::OnBattleActionCompleted(const FBattleAction& CompletedAction)
{
	static_cast<void>(CompletedAction);

	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor.Get());

	if (!IsValid(OwnerCharacter))
		return;

	UMuksiStatusEffectComponent* StatusEffectComponent = OwnerCharacter->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
		return;

	StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::WaterEnergy, 1, 1);
	StatusEffectComponent->SubtractStatusEffect(MuksiStatusEffectIDs::ChargeWaterEnergy, 1, 0);
}
