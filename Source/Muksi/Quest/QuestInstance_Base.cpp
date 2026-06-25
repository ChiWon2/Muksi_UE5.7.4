#include "QuestInstance_Base.h"
#include "QuestSubsystem.h"
//#include"../Inventory/InventorySubsystem.h"


void UQuestInstance_Base::InitializeQuestInstance(const FQuestKey& InQuestKey,const FQuestDetailRow& InQuestDetails,UQuestSubsystem* InQuestSubsystem)
{
    QuestKey = InQuestKey;

    QuestDetails = InQuestDetails;

    QuestState = EQuestState::Active;

    QuestSubsystem = InQuestSubsystem;

    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.AddDynamic(this,&UQuestInstance_Base::HandleObjectiveIDCalled);
    }

    //InventorySubsystem = UInventorySubsystem::Get(this);

    //if (InventorySubsystem)
    //{
    //    InventorySubsystem->OnItemChanged.AddDynamic(this,&UQuestInstance_Base::HandleItemChanged);
    //}

    InitializeObjectiveProgress(QuestDetails);
}

void UQuestInstance_Base::BeginDestroy()
{
    if (QuestSubsystem)
    {
        QuestSubsystem->OnObjectiveIDCalled.RemoveDynamic(this,&UQuestInstance_Base::HandleObjectiveIDCalled);
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

int32 UQuestInstance_Base::GetCurrentObjectiveProgress(const FObjectiveDetails& Objective) const
{
    // !!!!!!!!!!!!!!!!!TODO::InventorySubsystem에서 Item Count 조회!!!!!!!!!!!!!!!!!!!!!!
    if (Objective.Type == EObjectiveType::Collect)
    {
        /*UInventorySubsystem* InventorySubsystem =UInventorySubsystem::Get(this);*/

        /*if (!InventorySubsystem)
            return 0;*/

        //return InventorySubsystem->GetItemCount(Objective.TargetID);


        return 0;
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

    for (const FObjectiveDetails& Obj :InQuestDetails.Objectives)
    {
        // Collect는 실시간 조회 방식
        if (Obj.Type == EObjectiveType::Collect)
        {
            continue;
        }

        ObjectiveProgress.Add(Obj.ObjectiveID,0);
    }
}