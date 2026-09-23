#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "MuksiStatModifierStatusEffect.generated.h"

UCLASS(Abstract)
class MUKSI_API UMuksiStatModifierStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
	virtual void OnApplied() override;
	virtual void OnRemoved() override;

protected:
	virtual void OnStackChanged(int32 PreviousStack, int32 NewStack) override;
	virtual void ApplyStatModifier(int32 Amount) PURE_VIRTUAL(UMuksiStatModifierStatusEffect::ApplyStatModifier, );
};
