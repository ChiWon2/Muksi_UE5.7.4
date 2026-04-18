#include "GameConditionRegistry.h"

TMap<const UScriptStruct*, FConditionFunc>& FGameConditionRegistry::GetMap()
{
	static TMap<const UScriptStruct*, FConditionFunc> Map;
	return Map;
}

void FGameConditionRegistry::Register(const UScriptStruct* Type, FConditionFunc Func)
{
	GetMap().Add(Type, Func);
}

bool FGameConditionRegistry::Evaluate(UObject* WorldContext, const FInstancedStruct& Data)
{
	if (!Data.IsValid())
		return false;

	const UScriptStruct* Type = Data.GetScriptStruct();

	if (FConditionFunc* Func = GetMap().Find(Type))
	{
		return (*Func)(WorldContext, Data);
	}

	UE_LOG(LogTemp, Error, TEXT("[GameConditionRegistry]No handler for %s"), *Type->GetName());
	return false;
}