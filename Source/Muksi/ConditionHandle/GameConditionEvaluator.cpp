#include "GameConditionEvaluator.h"
#include "GameConditionRegistry.h"

bool FGameConditionEvaluator::Check(UObject* WorldContext, const FInstancedStruct& Data)
{
	if (!Data.IsValid())
	{
		return false;
	}

	const UScriptStruct* Type = Data.GetScriptStruct();

	if (FConditionFunc* Func = FGameConditionRegistry::Find(Type))
	{
		return (*Func)(WorldContext, Data);
	}

	UE_LOG(LogTemp, Error, TEXT("[ConditionEvaluator] No handler for %s"), *Type->GetName());
	return false;
}

bool FGameConditionEvaluator::CheckAll(UObject* WorldContext, const TArray<FInstancedStruct>& Conditions)
{
	for (const FInstancedStruct& Cond : Conditions)
	{
		if (!Check(WorldContext, Cond))
		{
			return false;
		}
	}

	return true;
}