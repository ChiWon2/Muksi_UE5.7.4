#include "QuestEntryWidget_ForTown.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include"../QuestSubsystem.h"
#include "../Quest/QuestInstance_Base.h"

void UQuestEntryWidget_ForTown::NativeConstruct()
{
    Super::NativeConstruct();

    if (BTN_QuestEntry)
    {
        BTN_QuestEntry->OnClicked.AddUniqueDynamic( this, &UQuestEntryWidget_ForTown::OnEntryButtonClicked);
    }

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsEnabled(false);
    }

    TXT_QuestName->SetText(QuestDetails.QuestName);

    //bool bIsReadyToComplete = QuestInstance->QuestState == EQuestState::ReadyToComplete;
    //CB_IsComplete->SetIsChecked(bIsReadyToComplete);
}

void UQuestEntryWidget_ForTown::NativeDestruct()
{
    if (BTN_QuestEntry)
    {
        BTN_QuestEntry->OnClicked.RemoveDynamic(this, &UQuestEntryWidget_ForTown::OnEntryButtonClicked);
    }

    Super::NativeDestruct();
}

void UQuestEntryWidget_ForTown::OnEntryButtonClicked()
{
    OnQuestSelected.Broadcast(QuestKey);
}

void UQuestEntryWidget_ForTown::InitWidget(const FQuestKey& InQuestKey)
{
    QuestKey = InQuestKey;
    QuestDetails = *UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);


}