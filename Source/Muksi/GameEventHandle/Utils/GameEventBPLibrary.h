#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameEventBPLibrary.generated.h"

UCLASS()
class MUKSI_API UGameEventBPLibrary: public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintCallable,Category = "Game Event",meta = (WorldContext = "WorldContextObject"))
    static void ExecuteStartBattleEvent(UObject* WorldContextObject, int32 BattleID);

    UFUNCTION(BlueprintCallable, Category = "Game Event", meta = (WorldContext = "WorldContextObject"))
    static void ExecuteAcceptQuestEvent(UObject* WorldContextObject, FName QuestID);
};