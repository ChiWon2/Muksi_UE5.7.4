#include "QuestObjectiveTarget.h"
#include "../QuestSubsystem.h"
#include "QuestObjectiveCharacter.h"

AQuestObjectiveTarget::AQuestObjectiveTarget()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AQuestObjectiveTarget::InitializeObjective(const FName& InObjectiveID, int32 InObjectiveValue)
{
    ObjectiveID = InObjectiveID;
    ObjectiveValue = InObjectiveValue;
}

void AQuestObjectiveTarget::NotifyObjectiveID()
{
    if (ObjectiveID.IsNone())
        return;

    if (UQuestSubsystem* QuestSubsystem = UQuestSubsystem::Get(this))
    {
        QuestSubsystem->OnObjectiveIDCalled.Broadcast(ObjectiveID, ObjectiveValue);
    }
}
