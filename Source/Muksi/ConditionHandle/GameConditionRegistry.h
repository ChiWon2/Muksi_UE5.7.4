#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

class UObject;

using FConditionFunc = TFunction<bool(UObject*, const FInstancedStruct&)>;

class FGameConditionRegistry
{
public:
	static TMap<const UScriptStruct*, FConditionFunc>& GetMap();

	static void Register(const UScriptStruct* Type, FConditionFunc Func);

	static FConditionFunc* Find(const UScriptStruct* Type);


	template<typename T>
	static void RegisterCondition(bool(*Func)(UObject*, const T&))
	{
		Register(T::StaticStruct(),
			[Func](UObject* WorldContext, const FInstancedStruct& Data)
			{
				return Func(WorldContext, Data.Get<T>());
			});
	}
};