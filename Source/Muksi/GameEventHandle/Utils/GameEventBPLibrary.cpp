#include "GameEventBPLibrary.h"
#include "GameEventUtils.h"

void UGameEventBPLibrary::ExecuteStartBattleEvent(UObject* WorldContextObject,int32 BattleID)
{
    GameEventUtils::ExecuteStartBattle(WorldContextObject,BattleID);
}

void UGameEventBPLibrary::ExecuteAcceptQuestEvent(UObject* WorldContextObject, FName QuestID)
{
    GameEventUtils::ExecuteAcceptQuest(WorldContextObject, QuestID);
}

void UGameEventBPLibrary::ExecuteObjectiveComplete(UObject* WorldContextObject, FName ObjectiveID)
{
    GameEventUtils::ExecuteObjectiveComplete(WorldContextObject, ObjectiveID);
}