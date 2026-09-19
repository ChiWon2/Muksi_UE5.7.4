#pragma once

#include "CoreMinimal.h"
#include "../MuksiStatusEffect.h"
#include "ChargeWaterEnergyStatusEffect.generated.h"

UCLASS()
class MUKSI_API UChargeWaterEnergyStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()

public:
	virtual void OnBattleActionCompleted(const FBattleAction& CompletedAction) override;
};
