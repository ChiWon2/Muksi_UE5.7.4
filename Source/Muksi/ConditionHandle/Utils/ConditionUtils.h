#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "../GameConditionEvaluator.h"

#include "../Condition/Cond_TravelTime.h"

namespace ConditionUtils
{
	FInstancedStruct MakeTravelTime(FTravelDate Date, EConditionCompareOp Op);
	bool CheckTravelTime(UObject* WorldContext, FTravelDate Date, EConditionCompareOp Op);
}