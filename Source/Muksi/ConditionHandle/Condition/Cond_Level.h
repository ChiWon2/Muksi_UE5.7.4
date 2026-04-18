#pragma once

#include "CoreMinimal.h"
#include"Cond_Base.h"
#include "../Types/ConditionCompareOp.h"
#include "../Types/ConditionTarget.h"
#include "Cond_Level.generated.h"

USTRUCT(BlueprintType)
struct FCond_Level : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionTarget Target = EConditionTarget::Self;

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionCompareOp CompareOp = EConditionCompareOp::GreaterOrEqual;

	UPROPERTY(EditAnywhere, Category = "Condition")
	int32 RequiredLevel = 0;
};