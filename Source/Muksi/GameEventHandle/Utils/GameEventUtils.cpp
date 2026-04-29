#include "GameEventUtils.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_StartBattle.h"
#include "../Events/Event_AcceptQuest.h"


FInstancedStruct GameEventUtils::MakeStartBattle(int32 BattleID)
{
    FEvent_StartBattle Event;

    Event.BattleID = BattleID;

    return FInstancedStruct::Make(Event);
}

void GameEventUtils::ExecuteStartBattle(UObject* WorldContext,int32 BattleID)
{
    UGameEventHandleSubsystem* Subsystem = UGameEventHandleSubsystem::Get(WorldContext);
    Subsystem->ExecuteEvent(WorldContext,MakeStartBattle(BattleID));
}


FInstancedStruct GameEventUtils::MakeAcceptQuest(FName QuestID)
{
    FEvent_AcceptQuest Event;

    Event.QuestID = QuestID;

    return FInstancedStruct::Make(Event);
}

void GameEventUtils::ExecuteAcceptQuest(UObject* WorldContext, FName QuestID)
{
    UGameEventHandleSubsystem* Subsystem = UGameEventHandleSubsystem::Get(WorldContext);
    Subsystem->ExecuteEvent(WorldContext, MakeAcceptQuest(QuestID));
}