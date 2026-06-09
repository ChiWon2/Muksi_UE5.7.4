#include "Handle_CompleteQuest.h"
#include"../Quest/QuestSubsystem.h"
#include "../GameEventHandleSubsystem.h"
#include "../Events/Event_CompleteQuest.h"


void FHandle_CompleteQuest::Handle_CompleteQuest(const UObject* WorldContext,const FEvent_CompleteQuest& EventParam)
{
	UE_LOG(LogTemp,Warning,TEXT("[Handle_CompleteQuest] : Complete QuestEvent Executed!! QuestID : %s"),*(EventParam.QuestID).ToString());
	UQuestSubsystem* QuestSubsys = UQuestSubsystem::Get(WorldContext);
	QuestSubsys->CompleteQuest(FQuestKey(EventParam.QuestTableID, EventParam.QuestID));
}

void FHandle_CompleteQuest::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_CompleteQuest>(Handle_CompleteQuest);
}
