#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameCondition.h"
#include "GameConditionBPLibrary.generated.h"

UCLASS()
class MUKSI_API UGameConditionBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Condition", meta = (WorldContext = "WorldContextObject"))
	static bool CheckCondition(UObject* WorldContextObject, const FGameCondition& Condition);

	UFUNCTION(BlueprintCallable, Category = "Condition", meta = (WorldContext = "WorldContextObject"))
	static bool CheckConditions(UObject* WorldContextObject, const TArray<FGameCondition>& Conditions);
};