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
        BTN_QuestEntry->OnClicked.AddUniqueDynamic(this, &UQuestEntryWidget_ForTown::OnEntryButtonClicked);
    }

    UQuestSubsystem* QuestSubsys = UQuestSubsystem::Get(this);
    QuestSubsys->OnQuestAccept.AddUniqueDynamic(this, &UQuestEntryWidget_ForTown::RefreshUIContent);
    
    RefreshUIContent();
}

void UQuestEntryWidget_ForTown::NativeDestruct()
{
    BTN_QuestEntry->OnClicked.RemoveDynamic(this, &UQuestEntryWidget_ForTown::OnEntryButtonClicked);
    UQuestSubsystem* QuestSubsys = UQuestSubsystem::Get(this);
    QuestSubsys->OnQuestAccept.RemoveDynamic(this, &UQuestEntryWidget_ForTown::RefreshUIContent);
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

void UQuestEntryWidget_ForTown::RefreshUIContent(UQuestInstance_Base* InInstance)
{
    if (TXT_QuestName)
    {
        TXT_QuestName->SetText(QuestDetails.QuestName);
    }

    if (!TXT_QuestProgress)
    {
        return;
    }

    UQuestInstance_Base* QuestInstance =
        UQuestSubsystem::Get(this)->GetActiveQuestInstance(QuestKey);

    if (QuestInstance)
    {
        if (QuestInstance->QuestState == EQuestState::ReadyToComplete)
        {
            TXT_QuestProgress->SetText(FText::FromString(TEXT("Complete")));
            TXT_QuestProgress->SetColorAndOpacity(
                FSlateColor(FLinearColor(0.0f, 1.0f, 0.0f))
            );
        }
        else
        {
            TXT_QuestProgress->SetText(FText::FromString(TEXT("Progress")));
            TXT_QuestProgress->SetColorAndOpacity(
                FSlateColor(FLinearColor(1.0f, 1.0f, 0.0f))
            );
        }
    }
    else
    {
        TXT_QuestProgress->SetText(FText::GetEmpty());
    }
}