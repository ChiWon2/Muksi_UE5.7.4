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

FConditionFunc* FGameConditionRegistry::Find(const UScriptStruct* Type)
{
	return GetMap().Find(Type);
}