#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"
#include "IceEnergyReleaseRuntimeModifier.generated.h"

UCLASS(Blueprintable)
class MUKSI_API UIceEnergyReleaseRuntimeModifier : public UBattleActionRuntimeModifier
{
	GENERATED_BODY()

public:
	UIceEnergyReleaseRuntimeModifier();

	virtual void ModifyBattleAction(FBattleAction& Action) const override;
};
