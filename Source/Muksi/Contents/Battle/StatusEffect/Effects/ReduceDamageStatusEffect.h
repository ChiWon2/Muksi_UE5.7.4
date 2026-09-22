#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiIncomingDamageModifierStatusEffect.h"
#include "ReduceDamageStatusEffect.generated.h"

UCLASS()
class MUKSI_API UReduceDamageStatusEffect : public UMuksiIncomingDamageModifierStatusEffect
{
	GENERATED_BODY()

public:
	virtual int32 GetIncomingDamageModifierPriority() const override;
	virtual void ModifyIncomingDamage(FIncomingDamageModifierContext& Context) override;
};
