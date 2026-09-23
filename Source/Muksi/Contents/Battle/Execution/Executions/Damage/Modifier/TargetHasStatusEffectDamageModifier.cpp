#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/TargetHasStatusEffectDamageModifier.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/TargetHasStatusEffectDamageModifierData.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

int32 UTargetHasStatusEffectDamageModifier::ModifyDamage(
	const FBattleExecutionContext&,
	ABattleCharacterBase* TargetCharacter,
	int32 Damage,
	const FInstancedStruct& ModifierData) const
{
	if (!TargetCharacter || !IsModifierDataValid(ModifierData))
		return Damage;

	const FTargetHasStatusEffectDamageModifierData* Data = ModifierData.GetPtr<FTargetHasStatusEffectDamageModifierData>();

	if (!Data || Data->StatusEffectID.IsNone())
		return Damage;

	UMuksiStatusEffectComponent* StatusEffectComponent = TargetCharacter->GetStatusEffectComponent();

	if (!StatusEffectComponent || !StatusEffectComponent->FindEffectByID(Data->StatusEffectID))
		return Damage;

	return Damage + Data->AdditionalDamage;
}

const UScriptStruct* UTargetHasStatusEffectDamageModifier::GetModifierDataStruct() const
{
	return FTargetHasStatusEffectDamageModifierData::StaticStruct();
}
