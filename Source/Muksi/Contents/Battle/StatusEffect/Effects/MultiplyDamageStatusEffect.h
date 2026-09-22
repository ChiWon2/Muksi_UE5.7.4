#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiIncomingDamageModifierStatusEffect.h"
#include "MultiplyDamageStatusEffect.generated.h"

UCLASS()
class MUKSI_API UMultiplyDamageStatusEffect : public UMuksiIncomingDamageModifierStatusEffect
{
	GENERATED_BODY()

public:
	virtual int32 GetIncomingDamageModifierPriority() const override;
	virtual void ModifyIncomingDamage(FIncomingDamageModifierContext& Context) override;
};
