#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/DamageModifier.h"

int32 UMuksiDamageModifier::ModifyDamage(
	const FBattleExecutionContext&,
	ABattleCharacterBase*,
	int32 Damage,
	const FInstancedStruct&) const
{
	return Damage;
}

const UScriptStruct* UMuksiDamageModifier::GetModifierDataStruct() const
{
	return nullptr;
}

bool UMuksiDamageModifier::IsModifierDataValid(const FInstancedStruct& ModifierData) const
{
	const UScriptStruct* ExpectedStruct = GetModifierDataStruct();

	if (!ExpectedStruct)
		return !ModifierData.IsValid();

	return ModifierData.GetScriptStruct() == ExpectedStruct;
}
