#pragma once

#include "CoreMinimal.h"
#include"Cond_Base.h"
#include "../Types/ConditionTarget.h"
#include "Cond_Equip.generated.h"

USTRUCT(BlueprintType)
struct FCond_Equip : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionTarget Target = EConditionTarget::Self;

	UPROPERTY(EditAnywhere, Category = "Condition")
	FName ItemID;
};