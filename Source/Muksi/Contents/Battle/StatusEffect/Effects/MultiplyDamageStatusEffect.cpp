#include "Muksi/Contents/Battle/StatusEffect/Effects/MultiplyDamageStatusEffect.h"

int32 UMultiplyDamageStatusEffect::GetIncomingDamageModifierPriority() const
{
	return 400;
}

void UMultiplyDamageStatusEffect::ModifyIncomingDamage(FIncomingDamageModifierContext& Context)
{
	if (Context.RemainingDamage <= 0)
		return;

	const float DamageRate = 1.0f + static_cast<float>(GetCurrentStack()) / 100.0f;
	Context.RemainingDamage = FMath::Max(0, FMath::RoundToInt(static_cast<float>(Context.RemainingDamage) * DamageRate));
}
