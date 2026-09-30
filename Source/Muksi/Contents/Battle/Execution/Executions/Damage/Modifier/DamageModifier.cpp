#include "Muksi/Contents/Battle/Execution/Executions/Damage/Modifier/DamageModifier.h"

int32 UMuksiDamageModifier::ModifyDamage(
	const FBattleExecutionContext&,
	ABattleCharacterBase*,
	int32 Damage) const
{
	return Damage;
}
