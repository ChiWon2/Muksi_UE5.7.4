#include "GameConditionBPLibrary.h"
#include "GameConditionEvaluator.h"

bool UGameConditionBPLibrary::CheckCondition(UObject* WorldContextObject, const FGameCondition& Condition)
{
	return FGameConditionEvaluator::Check(WorldContextObject, Condition);
}

bool UGameConditionBPLibrary::CheckConditions(UObject* WorldContextObject, const TArray<FGameCondition>& Conditions)
{
	return FGameConditionEvaluator::CheckAll(WorldContextObject, Conditions);
}