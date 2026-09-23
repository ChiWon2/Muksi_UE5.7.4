#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/TargetHasStatusEffectDamageModifier.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

int32 UTargetHasStatusEffectDamageModifier::ModifyDamage(
	const FBattleExecutionContext&,
	ABattleCharacterBase* TargetCharacter,
	int32 Damage) const
{
	if (!TargetCharacter || StatusEffectID.IsNone())
		return Damage;

	UMuksiStatusEffectComponent* StatusEffectComponent = TargetCharacter->GetStatusEffectComponent();

	if (!StatusEffectComponent || !StatusEffectComponent->FindEffectByID(StatusEffectID))
		return Damage;

	return Damage + AdditionalDamage;
}
