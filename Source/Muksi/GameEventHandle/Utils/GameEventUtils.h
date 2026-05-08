#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"

struct FEvent_StartBattle;
struct FEvent_AcceptQuest;

namespace GameEventUtils
{
    FInstancedStruct MakeStartBattle(int32 BattleID);

    void ExecuteStartBattle(UObject* WorldContext,int32 BattleID);

    FInstancedStruct MakeAcceptQuest(FName QuestID);

    void ExecuteAcceptQuest(UObject* WorldContext, FName QuestID);
}