#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "GameConditionEvaluator.generated.h"

USTRUCT()
struct FGameConditionEvaluator
{
	GENERATED_BODY()

public:
	static bool Check(UObject* WorldContext, const FInstancedStruct& Data);

	static bool CheckAll(UObject* WorldContext, const TArray<FInstancedStruct>& Conditions);
};