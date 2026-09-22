#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiIncomingDamageModifierStatusEffect.h"
#include "ShieldStatusEffect.generated.h"

UCLASS()
class MUKSI_API UShieldStatusEffect : public UMuksiIncomingDamageModifierStatusEffect
{
	GENERATED_BODY()

public:
	virtual int32 GetIncomingDamageModifierPriority() const override;
	virtual void ModifyIncomingDamage(FIncomingDamageModifierContext& Context) override;
	virtual FName GetHitReactionAnimKey() const override;
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Defense")
	bool bExpireAtRoundEnd = true;
};
