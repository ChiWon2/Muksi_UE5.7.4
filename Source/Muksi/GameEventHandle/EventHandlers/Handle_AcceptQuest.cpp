#include "Handle_AcceptQuest.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_AcceptQuest.h"


void FHandle_AcceptQuest::Handle_AcceptQuest(const UObject* WorldContext,const FEvent_AcceptQuest& Event)
{
	UE_LOG(LogTemp,Warning,TEXT("[Handle_AcceptQuest] : AcceptQuestEvent Executed!! QuestID : \"%s\" is Add into your QuestSystem"),*(Event.QuestID).ToString());
}

void FHandle_AcceptQuest::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_AcceptQuest>(Handle_AcceptQuest);
}
