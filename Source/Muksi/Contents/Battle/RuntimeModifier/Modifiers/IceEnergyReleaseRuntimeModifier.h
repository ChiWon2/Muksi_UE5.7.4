#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"
#include "IceEnergyReleaseRuntimeModifier.generated.h"

UCLASS(Blueprintable)
class MUKSI_API UIceEnergyReleaseRuntimeModifier : public UBattleActionRuntimeModifier
{
	GENERATED_BODY()

public:
	virtual void ModifyBattleAction(FBattleAction& Action) const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ice Energy Combo|Animation", meta = (ClampMin = "0.0"))
	float ComboPlayRate = 1.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ice Energy Combo|Combo 1", meta = (ClampMin = "0"))
	int32 Combo1Damage = 8;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ice Energy Combo|Combo 2", meta = (ClampMin = "0"))
	int32 Combo2Damage = 15;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ice Energy Combo|Combo 3", meta = (ClampMin = "0"))
	int32 Combo3Damage = 30;
};
