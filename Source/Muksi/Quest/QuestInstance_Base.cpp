#include "QuestInstance_Base.h"
#include "QuestSubsystem.h"
#include"Subsystems/MuksiPlayerDataSubsystem.h"
#include "../Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "../Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "../Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

void UQuestInstance_Base::InitializeQuestInstance(const FQuestKey& InQuestKey,const FQuestDetailRow& InQuestDetails,UQuestSubsystem* InQuestSubsystem)
{
    QuestKey = InQuestKey;

    QuestDetails = InQuestDetails;

    QuestState = EQuestState::Active;
    
    QuestSubsystem = UQuestSubsystem::Get(this);
    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.AddUniqueDynamic(this,&UQuestInstance_Base::HandleObjectiveIDCalled);
    }

    PlayerInventory = UMuksiPlayerDataSubsystem::Get(this)->GetPlayerInventoryComponent();
    if (PlayerInventory)
    {
        PlayerInventory->OnInventoryChanged.AddUniqueDynamic(this,&UQuestInstance_Base::HandlePlayerInventoryChanged);
    }

    InitializeObjectiveProgress(QuestDetails);
}

void UQuestInstance_Base::BeginDestroy()
{
    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.RemoveDynamic(this,&UQuestInstance_Base::HandleObjectiveIDCalled);
    }
    if (PlayerInventory)
    {
        PlayerInventory->OnInventoryChanged.RemoveDynamic(this, &UQuestInstance_Base::HandlePlayerInventoryChanged);
    }

    Super::BeginDestroy();
}

void UQuestInstance_Base::HandleObjectiveIDCalled(FName ObjectiveID,int32 Value)
{
    const FObjectiveDetails* FoundObjective = nullptr;

    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        if (Obj.ObjectiveID == ObjectiveID)
        {
            FoundObjective = &Obj;
            break;
        }
    }

    if (!FoundObjective)
        return;

    // Collect는 Inventory 기반으로 처리
    if (FoundObjective->Type == EObjectiveType::Collect)
        return;

    if (!ObjectiveProgress.Contains(ObjectiveID))
        return;

    ObjectiveProgress[ObjectiveID] += Value;

    OnObjectiveUpdated.Broadcast(ObjectiveID,ObjectiveProgress[ObjectiveID]);

    if (AreAllObjectivesComplete())
    {
        QuestState = EQuestState::ReadyToComplete;

        UE_LOG(LogTemp,Warning,TEXT("[QuestInstance] Quest is ReadyToComplete : %s"),*QuestKey.ToString());
    }
}

void UQuestInstance_Base::HandlePlayerInventoryChanged(FName ItemID, int32 OldCount, int32 NewCount)
{
    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        if (Obj.Type != EObjectiveType::Collect)
        {
            continue;
        }
        if (Obj.ObjectiveID != ItemID)
        {
            continue;
        }
        ObjectiveProgress.FindOrAdd(Obj.ObjectiveID) = NewCount;

        OnObjectiveUpdated.Broadcast(Obj.ObjectiveID, NewCount);
    }

    if (AreAllObjectivesComplete())
    {
        QuestState = EQuestState::ReadyToComplete;
    }
}

int32 UQuestInstance_Base::GetCurrentObjectiveProgress(const FObjectiveDetails& Objective) const
{
    if (Objective.Type == EObjectiveType::Collect)
    {
        if (PlayerInventory == nullptr)
        {
            return 0;
        }
        return PlayerInventory->GetItemCountByItemID(Objective.ObjectiveID);
    }

    else
    {
        const int32* Value = ObjectiveProgress.Find(Objective.ObjectiveID);

        return Value ? *Value : 0;
    }
}

const FObjectiveDetails* UQuestInstance_Base::FindObjective(const FName& ObjectiveID) const
{
    for (const FObjectiveDetails& Obj : QuestDetails.Objectives)
    {
        if (Obj.ObjectiveID == ObjectiveID)
        {
            return &Obj;
        }
    }
    return nullptr;
}

bool UQuestInstance_Base::IsObjectiveComplete(const FName& ObjectiveID) const
{
    for (const FObjectiveDetails& Obj :QuestDetails.Objectives)
    {
        if (Obj.ObjectiveID != ObjectiveID)
            continue;

        return GetCurrentObjectiveProgress(Obj)>= Obj.RequiredQuantity;
    }
    return false;
}

bool UQuestInstance_Base::AreAllObjectivesComplete() const
{
    for (const FObjectiveDetails& Obj :QuestDetails.Objectives)
    {
        if (!IsObjectiveComplete(Obj.ObjectiveID))
        {
            return false;
        }
    }

    return true;
}

void UQuestInstance_Base::InitializeObjectiveProgress(const FQuestDetailRow& InQuestDetails)
{
    ObjectiveProgress.Empty();

    for (const FObjectiveDetails& Obj : InQuestDetails.Objectives)
    {
        int32 InitialProgress = 0;

        if (Obj.Type == EObjectiveType::Collect && PlayerInventory)
        {
            InitialProgress = PlayerInventory->GetItemCountByItemID(Obj.ObjectiveID);
        }

        ObjectiveProgress.Add(Obj.ObjectiveID,InitialProgress);
    }
}

