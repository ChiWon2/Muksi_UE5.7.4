#include "MuksiStatModifierStatusEffect.h"

void UMuksiStatModifierStatusEffect::OnApplied()
{
	Super::OnApplied();

	ApplyStatModifier(GetCurrentStack());
}

void UMuksiStatModifierStatusEffect::OnRemoved()
{
	ApplyStatModifier(-GetCurrentStack());

	Super::OnRemoved();
}

void UMuksiStatModifierStatusEffect::OnStackChanged(int32 PreviousStack, int32 NewStack)
{
	Super::OnStackChanged(PreviousStack, NewStack);

	ApplyStatModifier(NewStack - PreviousStack);
}
