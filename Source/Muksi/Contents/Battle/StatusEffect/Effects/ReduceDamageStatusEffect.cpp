#include "Muksi/Contents/Battle/StatusEffect/Effects/ReduceDamageStatusEffect.h"

int32 UReduceDamageStatusEffect::GetIncomingDamageModifierPriority() const
{
	return 400;
}

void UReduceDamageStatusEffect::ModifyIncomingDamage(FIncomingDamageModifierContext& Context)
{
	if (Context.RemainingDamage <= 0)
		return;

	const float DamageRate = 1.0f - FMath::Clamp(static_cast<float>(GetCurrentStack()) / 100.0f, 0.0f, 1.0f);
	Context.RemainingDamage = FMath::Max(0, FMath::RoundToInt(static_cast<float>(Context.RemainingDamage) * DamageRate));
}
