#include "Handle_AcceptQuest.h"
#include"../Quest/QuestSubsystem.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_AcceptQuest.h"


void FHandle_AcceptQuest::Handle_AcceptQuest(const UObject* WorldContext,const FEvent_AcceptQuest& EventParam)
{
	UE_LOG(LogTemp,Warning,TEXT("[Handle_AcceptQuest] : AcceptQuestEvent Executed!! QuestID : \"%s\" is Add into your QuestSystem"),*(EventParam.QuestID).ToString());
	UQuestSubsystem* QuestSubsys = UQuestSubsystem::Get(WorldContext);
	QuestSubsys->AddNewQuest(FQuestKey(EventParam.QuestTableID, EventParam.QuestID));
}

void FHandle_AcceptQuest::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_AcceptQuest>(Handle_AcceptQuest);
}
