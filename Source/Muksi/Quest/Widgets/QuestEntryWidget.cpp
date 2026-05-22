#include "QuestEntryWidget.h"

#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/TextBlock.h"

#include "../Quest/QuestInstance_Base.h"

void UQuestEntryWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BTN_QuestEntry)
    {
        BTN_QuestEntry->OnClicked.AddDynamic( this, &UQuestEntryWidget::OnEntryButtonClicked);
    }

    if (BTN_Track)
    {
        BTN_Track->OnClicked.AddDynamic( this, &UQuestEntryWidget::OnTrackButtonClicked);
    }

    if (CB_IsComplete)
    {
        CB_IsComplete->SetIsEnabled(false);
    }

    if (!QuestInstance)
        return;

    TXT_QuestName->SetText( QuestInstance->QuestDetails.QuestName);

    CB_IsComplete->SetIsChecked(QuestInstance->bIsCompleted);
}

void UQuestEntryWidget::NativeDestruct()
{
    Super::NativeDestruct();
}

void UQuestEntryWidget::OnEntryButtonClicked()
{
    OnQuestSelected.Broadcast(QuestInstance);
}

void UQuestEntryWidget::OnTrackButtonClicked()
{
    OnTrackSelected.Broadcast(QuestInstance);
}

void UQuestEntryWidget::InitWidget(UQuestInstance_Base* InQuestInstance)
{
    QuestInstance = InQuestInstance;
}