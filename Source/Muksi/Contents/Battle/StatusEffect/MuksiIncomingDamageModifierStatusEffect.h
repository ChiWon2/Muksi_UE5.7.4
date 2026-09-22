#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Executions/Damage/IncomingDamageModifierTypes.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "MuksiIncomingDamageModifierStatusEffect.generated.h"


UCLASS(Abstract, BlueprintType)
class MUKSI_API UMuksiIncomingDamageModifierStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
	virtual int32 GetIncomingDamageModifierPriority() const PURE_VIRTUAL(UMuksiIncomingDamageModifierStatusEffect::GetIncomingDamageModifierPriority, return 0;);
	virtual void ModifyIncomingDamage(FIncomingDamageModifierContext& Context) PURE_VIRTUAL(UMuksiIncomingDamageModifierStatusEffect::ModifyIncomingDamage, );
	virtual FName GetHitReactionAnimKey() const;
};
