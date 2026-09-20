#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleActionRuntimeModifier.generated.h"

struct FBattleAction;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UBattleActionRuntimeModifier : public UObject
{
	GENERATED_BODY()

public:
	virtual void ModifyBattleAction(FBattleAction& Action) const;
};
