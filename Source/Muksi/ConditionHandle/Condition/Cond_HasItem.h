#pragma once

#include "CoreMinimal.h"
#include"Cond_Base.h"
#include "../Types/ConditionCompareOp.h"
#include "Cond_HasItem.generated.h"

USTRUCT(BlueprintType)
struct FCond_HasItem : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	FName ItemID;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionCompareOp CompareOp = EConditionCompareOp::GreaterOrEqual;

	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 Count = 1;

};