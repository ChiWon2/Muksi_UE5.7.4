#include "Base_QuestInstance.h"
#include"QuestSubsystem.h"

void UBase_QuestInstance::InitializeQuestInstance(const FName& InQuestID,const FQuestDetailRow& InQuestDetails, UQuestSubsystem* InQuestSubsystem)
{
    QuestID = InQuestID;
    QuestDetails = InQuestDetails;
    bIsCompleted = false;


    QuestSubsystem = InQuestSubsystem;
    QuestSubsystem->OnObjectiveIDCalled.AddDynamic(this, &UBase_QuestInstance::HandleObjectiveIDCalled);

    InitializeObjectiveProgress(QuestDetails);
}

void UBase_QuestInstance::BeginDestroy()
{
    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.RemoveDynamic(this,&UBase_QuestInstance::HandleObjectiveIDCalled);
    }
    Super::BeginDestroy();
}

void UBase_QuestInstance::HandleObjectiveIDCalled(const FName& ObjectiveID,int32 Value)
{
    if (!ObjectiveProgress.Contains(ObjectiveID))
        return;

    ObjectiveProgress[ObjectiveID] += Value;

    OnObjectiveUpdated.Broadcast(ObjectiveID, ObjectiveProgress[ObjectiveID]);

    if (AreAllObjectivesComplete())
    {
        bIsCompleted = true;
        QuestSubsystem->CompleteQuest(QuestID);
        UE_LOG(LogTemp, Warning, TEXT("[Base_QuestInstance] %s Objective is Completed!"),*ObjectiveID.ToString());
    }
}

bool UBase_QuestInstance::IsObjectiveComplete(const FName& ObjectiveID) const
{
    const int32* CurrentValue = ObjectiveProgress.Find(ObjectiveID);

    if (!CurrentValue)
        return false;

    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        if (Obj.ObjectiveID == ObjectiveID)
        {
            return (*CurrentValue >= Obj.RequiredQuantity);
        }
    }
    return false;
}

bool UBase_QuestInstance::AreAllObjectivesComplete() const
{
    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        const int32* Value = ObjectiveProgress.Find(Obj.ObjectiveID);
        if (!Value || *Value < Obj.RequiredQuantity)
        {
            return false;
        }
    }
    return true;
}

void UBase_QuestInstance::InitializeObjectiveProgress(const FQuestDetailRow& InQuestDetails)
{
    ObjectiveProgress.Empty();

    for (const FObjectiveDetails& Obj : InQuestDetails.Objectives)
    {
        ObjectiveProgress.Add(Obj.ObjectiveID, 0);
    }
}
