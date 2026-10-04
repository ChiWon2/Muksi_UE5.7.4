#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/ExecutionModifyStatusEffect.h"
#include "SaeMaekStatusEffect.generated.h"

UCLASS()
class MUKSI_API USaeMaekStatusEffect : public UExecutionModifyStatusEffect
{
	GENERATED_BODY()

public:
	USaeMaekStatusEffect();

	virtual void EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction) override;
};
