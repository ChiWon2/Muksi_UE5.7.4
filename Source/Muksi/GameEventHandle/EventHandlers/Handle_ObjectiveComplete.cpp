#include "Handle_ObjectiveComplete.h"
#include "../GameEventHandleSubsystem.h"
#include"../../Quest/QuestSubsystem.h"
#include "../Events/Event_ObjectiveComplete.h"


void FHandle_ObjectiveComplete::Handle_ObjectiveComplete(const UObject* WorldContext,const FEvent_ObjectiveComplete& Event)
{
    UQuestSubsystem::Get(WorldContext)->OnObjectiveIDCalled.Broadcast(Event.ObjectiveID,1);
    UE_LOG(LogTemp, Warning, TEXT("[Handle_ObjectiveComplete] ObjectiveComplete : %s"),*Event.ObjectiveID.ToString());
}

void FHandle_ObjectiveComplete::Register(UGameEventHandleSubsystem& Subsystem)
{
    Subsystem.RegisterEvent<FEvent_ObjectiveComplete>(Handle_ObjectiveComplete);
}
