#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "StructUtils/InstancedStruct.h"
#include"../Condition/Cond_TravelTime.h"

#include "GameConditionBPLibrary.generated.h"

UCLASS()
class MUKSI_API UGameConditionBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Condition",meta = (WorldContext = "WorldContextObject",BaseStruct = "/Script/Muksi.Cond_Base"))	
	static bool CheckCondition(UObject* WorldContextObject, const FInstancedStruct& Condition);

	// 여러 조건 (AND)
	UFUNCTION(BlueprintCallable, Category = "Condition", meta = (WorldContext = "WorldContextObject"))
	static bool CheckConditions(UObject* WorldContextObject, const TArray<FInstancedStruct>& Conditions);

	UFUNCTION(BlueprintCallable, Category = "Condition", meta = (WorldContext = "WorldContextObject"))
	static bool CheckTravelTimeCondition(UObject* WorldContextObject,const FTravelDate& Date,EConditionCompareOp CompareOp);
};