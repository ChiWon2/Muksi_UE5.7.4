#pragma once

#include "CoreMinimal.h"
#include "../Types/ConditionCompareOp.h"
#include"Cond_Base.h"
#include"../../TravelTime/TravelTimeTypes.h"
#include "Cond_TravelTime.generated.h"

USTRUCT(BlueprintType)
struct FCond_TravelTime : public FCond_Base
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Condition")
	FTravelDate Date = FTravelDate(0,0,0,0);

	UPROPERTY(EditAnywhere, Category = "Condition")
	EConditionCompareOp CompareOp = EConditionCompareOp::GreaterOrEqual;
};