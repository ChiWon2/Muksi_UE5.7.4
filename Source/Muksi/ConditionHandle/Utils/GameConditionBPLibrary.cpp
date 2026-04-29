#include "GameConditionBPLibrary.h"
#include "../GameConditionEvaluator.h"



bool UGameConditionBPLibrary::CheckCondition(UObject* WorldContextObject, const FInstancedStruct& Condition)
{
	return FGameConditionEvaluator::Check(WorldContextObject, Condition);
}

bool UGameConditionBPLibrary::CheckConditions(UObject* WorldContextObject, const TArray<FInstancedStruct>& Conditions)
{
	return FGameConditionEvaluator::CheckAll(WorldContextObject, Conditions);
}

bool UGameConditionBPLibrary::CheckTravelTimeCondition(UObject* WorldContextObject, const FTravelDate& Date, EConditionCompareOp CompareOp)
{
    if (!WorldContextObject)
        return false;

    FCond_TravelTime Cond;
    Cond.Date = Date;
    Cond.CompareOp = CompareOp;

    return CheckCondition(WorldContextObject,FInstancedStruct::Make(Cond));
}
