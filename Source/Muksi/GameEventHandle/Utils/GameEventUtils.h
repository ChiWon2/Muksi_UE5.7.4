#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

struct FEvent_StartBattle;
struct FEvent_AcceptQuest;

namespace GameEventUtils
{
    //StartBattle
    FInstancedStruct MakeStartBattle(int32 BattleID);

    void ExecuteStartBattle(UObject* WorldContext,int32 BattleID);

    //AcceptQuest
    FInstancedStruct MakeAcceptQuest(FName QuestID);

    void ExecuteAcceptQuest(UObject* WorldContext, FName QuestID);
    
    //ObjectiveComplete
    FInstancedStruct MakeObjectiveComplete(FName ObjectiveID);

    void ExecuteObjectiveComplete(UObject* WorldContext, FName ObjectiveID);
}