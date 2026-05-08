#pragma once

#include "CoreMinimal.h"
#include "../Types/ConditionCompareOp.h"
#include "../Types/ConditionTarget.h"
#include"Cond_Base.h"
#include "Cond_Stat.generated.h"

USTRUCT(BlueprintType)
struct FCond_Stat : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionTarget Target = EConditionTarget::Self;

	UPROPERTY(EditAnywhere, Category = "Condition")
	FName StatName;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionCompareOp CompareOp = EConditionCompareOp::GreaterOrEqual;

	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 Value = 0;
};