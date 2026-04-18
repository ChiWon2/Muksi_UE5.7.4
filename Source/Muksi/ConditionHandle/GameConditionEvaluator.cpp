#include "GameConditionEvaluator.h"
#include "GameConditionRegistry.h"

bool FGameConditionEvaluator::Check(UObject* WorldContext, const FInstancedStruct& Data)
{
	return FGameConditionRegistry::Evaluate(WorldContext, Data);
}

bool FGameConditionEvaluator::CheckAll(UObject* WorldContext, const TArray<FInstancedStruct>& Conditions)
{
	for (const FInstancedStruct& Cond : Conditions)
	{
		if (!Check(WorldContext, Cond))
			return false;
	}
	return true;
}