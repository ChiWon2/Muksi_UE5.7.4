#pragma once

#include "CoreMinimal.h"
#include "../MuksiStatusEffect.h"
#include "ChargeIceEnergyStatusEffect.generated.h"

UCLASS()
class MUKSI_API UChargeIceEnergyStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
	virtual void OnBattleActionCompleted(const FBattleAction& CompletedAction) override;
};
