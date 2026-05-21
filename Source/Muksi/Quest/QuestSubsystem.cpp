#include "QuestSubsystem.h"

UQuestSubsystem* UQuestSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UQuestSubsystem>() : nullptr;
}

void UQuestSubsystem::InitializeQuestSubsystem(UDataTable* InQuestDataTable)
{
    QuestDataTable = InQuestDataTable;
}

UBase_QuestInstance* UQuestSubsystem::AddNewQuest(const FName& QuestID)
{
    if (ActiveQuests.Contains(QuestID) ||CompletedQuests.Contains(QuestID))
    {
        UE_LOG(LogTemp, Error, TEXT("[QuestSubsystem] : Already Has QuestID : %s"), *QuestID.ToString());
        return nullptr;
    }

    if (!QuestDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuestSubsystem] : There is No DataTable References!"));
        return nullptr;
    }

    const FQuestDetailRow* QuestRow =QuestDataTable->FindRow<FQuestDetailRow>(QuestID,TEXT("AddNewQuest"));

    if (!QuestRow)
    {
        UE_LOG(LogTemp, Error, TEXT("[QuestSubsystem] : Cannot Found QuestID : %s in the DataTable Row"), *QuestID.ToString());
        return nullptr;
    }

    UBase_QuestInstance* NewQuest = NewObject<UBase_QuestInstance>(this);

    NewQuest->InitializeQuestInstance(QuestID, *QuestRow , this);

    ActiveQuests.Add(QuestID, NewQuest);

    UE_LOG(LogTemp, Warning, TEXT("[QuestSubsystem] QuestID : %s is Accept !!"), *QuestID.ToString());

    return NewQuest;
}

void UQuestSubsystem::CompleteQuest(const FName& QuestID)
{
    UBase_QuestInstance* Quest = ActiveQuests.FindRef(QuestID);

    if (!Quest)
        return;

    // TODO : Reward 지급 내가 나중에 할거임
    UE_LOG(LogTemp,Warning,TEXT("[QuestSubsystem] : QuestID %s is Completed!! %i XPReward Achieved"), *QuestID.ToString(), Quest->QuestDetails.Reward.XPReward);
    ActiveQuests.Remove(QuestID);
    CompletedQuests.Add(QuestID);

    OnQuestCompleted.Broadcast(GetQuestInstance(QuestID));
}

bool UQuestSubsystem::IsQuestActive(const FName& QuestID) const
{
    return ActiveQuests.Contains(QuestID);
}

bool UQuestSubsystem::IsQuestCompleted(const FName& QuestID) const
{
    return CompletedQuests.Contains(QuestID);
}

UBase_QuestInstance* UQuestSubsystem::GetQuestInstance(const FName& QuestID) const
{
    return ActiveQuests.FindRef(QuestID);
}


