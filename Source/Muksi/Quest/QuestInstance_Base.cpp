#include "QuestInstance_Base.h"
#include "QuestSubsystem.h"

void UQuestInstance_Base::InitializeQuestInstance(const FQuestKey& InQuestKey, const FQuestDetailRow& InQuestDetails, UQuestSubsystem* InQuestSubsystem)
{
    QuestKey = InQuestKey;

    QuestDetails = InQuestDetails;

    QuestState  = EQuestState::Active;

    QuestSubsystem = InQuestSubsystem;

    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.AddDynamic(this, &UQuestInstance_Base::HandleObjectiveIDCalled);
    }

    InitializeObjectiveProgress(QuestDetails);
}

void UQuestInstance_Base::BeginDestroy()
{
    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.RemoveDynamic( this, &UQuestInstance_Base::HandleObjectiveIDCalled);
    }

    Super::BeginDestroy();
}

void UQuestInstance_Base::HandleObjectiveIDCalled(FName  ObjectiveID, int32 Value)
{
    if (!ObjectiveProgress.Contains(ObjectiveID))
        return;

    ObjectiveProgress[ObjectiveID] += Value;

    OnObjectiveUpdated.Broadcast( ObjectiveID, ObjectiveProgress[ObjectiveID]);

    if (AreAllObjectivesComplete())
    {
        QuestState = EQuestState::ReadyToComplete;

        /* Refactoring this Func Maybe
        if (QuestSubsystem)
        {
            QuestSubsystem->CompleteQuest(QuestKey);
        }
        */
        UE_LOG(LogTemp, Warning, TEXT("[QuestInstance] Quest Completed : %s"), *QuestKey.ToString());
    }
}

bool UQuestInstance_Base::IsObjectiveComplete( const FName& ObjectiveID) const
{
    const int32* CurrentValue = ObjectiveProgress.Find(ObjectiveID);

    if (!CurrentValue)
        return false;

    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        if (Obj.ObjectiveID == ObjectiveID)
        {
            return *CurrentValue >= Obj.RequiredQuantity;
        }
    }

    return false;
}

bool UQuestInstance_Base::AreAllObjectivesComplete() const
{
    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        const int32* Value =
            ObjectiveProgress.Find(Obj.ObjectiveID);

        if (!Value || *Value < Obj.RequiredQuantity)
        {
            return false;
        }
    }

    return true;
}

void UQuestInstance_Base::InitializeObjectiveProgress(const FQuestDetailRow& InQuestDetails)
{
    ObjectiveProgress.Empty();

    for (const FObjectiveDetails& Obj :
        InQuestDetails.Objectives)
    {
        ObjectiveProgress.Add( Obj.ObjectiveID, 0);
    }
}