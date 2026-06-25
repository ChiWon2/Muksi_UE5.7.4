#include "QuestLogWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

#include "Kismet/GameplayStatics.h"

#include "../Quest/QuestDetailRow.h"
#include "../Quest/QuestInstance_Base.h"
#include "../QuestSubsystem.h"
#include"../TravelTime/TravelTimeSubsystem.h"
#include "TabButton.h"
#include "QuestEntryWidget.h"
#include "QuestObjectiveEntryWidget.h"
#include "RewardsWidget.h"
#include "ObjectivesWidget.h"

void UQuestLogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TAB_Ongoing->SetTabText(FText::FromString("Quests"));
    TAB_Completed->SetTabText(FText::FromString("Completed"));
    TAB_Ongoing->SetSelected(true);
    TAB_Completed->SetSelected(false);

    HideQuestDetail();

    TAB_Ongoing->OnTabClicked.AddUniqueDynamic( this, &ThisClass::HandleTabClicked);

    TAB_Completed->OnTabClicked.AddUniqueDynamic( this, &ThisClass::HandleTabClicked);

    BT_CloseMenu->OnClicked.AddUniqueDynamic(this, &UQuestLogWidget::OnCloseButtonClicked);

}

void UQuestLogWidget::NativeDestruct()
{
    TAB_Ongoing->OnTabClicked.RemoveDynamic(this, &ThisClass::HandleTabClicked);
    TAB_Completed->OnTabClicked.RemoveDynamic(this, &ThisClass::HandleTabClicked);
    BT_CloseMenu->OnClicked.RemoveDynamic(this, &UQuestLogWidget::OnCloseButtonClicked);

    Super::NativeDestruct();
}

void UQuestLogWidget::NativeOnActivated()
{
    Super::NativeOnActivated();
    HideQuestDetail();
    UpdateQuestScrollBoxs();

    UTravelTimeSubsystem::Get(this)->StopTravelTime();

}

void UQuestLogWidget::NativeOnDeactivated()
{
    UTravelTimeSubsystem::Get(this)->StartTravelTime();
    Super::NativeOnDeactivated();
}

void UQuestLogWidget::OnCloseButtonClicked()
{
    DeactivateWidget();
}

void UQuestLogWidget::UpdateQuestScrollBoxs()
{
    UQuestSubsystem* QuestSubsystem = UQuestSubsystem::Get(this);

    if (!QuestSubsystem)
        return;

    SB_ActiveQuests->ClearChildren();
    SB_CompletedQuests->ClearChildren();

    for (const auto& Pair : QuestSubsystem->GetActiveQuests())
    {
        AddQuestEntryToScrollBox(Pair.Value, SB_ActiveQuests);
    }
    for (const auto& Pair : QuestSubsystem->GetCompletedQuests())
    {
        AddQuestEntryToScrollBox(Pair.Value, SB_CompletedQuests);
    }
}

void UQuestLogWidget::HandleTabClicked(UTabButton* ClickedTab)
{
    TAB_Ongoing->SetSelected(false);
    TAB_Completed->SetSelected(false);

    ClickedTab->SetSelected(true);

    HideQuestDetail();

    if (ClickedTab == TAB_Ongoing)
    {
        WS_QuestList->SetActiveWidgetIndex(0);
    }
    else
    {
        WS_QuestList->SetActiveWidgetIndex(1);
    }
}

void UQuestLogWidget::HandleQuestEntrySelected(UQuestInstance_Base* QuestInstance)
{
    UpdateQuestDetails(QuestInstance);
}

void UQuestLogWidget::UpdateQuestDetails(UQuestInstance_Base* QuestInstance)
{
    if (!QuestInstance)
        return;

    FQuestKey& QuestKey = QuestInstance->QuestKey;

    WS_ShowQuestDetail->SetActiveWidgetIndex(1);

    const FQuestDetailRow& QuestDetails = QuestInstance->QuestDetails;

    TXT_QuestName->SetText(QuestDetails.QuestName);

    TXT_QuestDescription->SetText(QuestDetails.Description);

    RewardsWidget->InitWidget(QuestKey);

    ObjectivesWidget->InitWidget(QuestDetails.Objectives, QuestInstance);
}

void UQuestLogWidget::HandleTrackSelected(UQuestInstance_Base* QuestInstance)
{
    if (!QuestInstance)
        return;

    UE_LOG( LogTemp, Warning, TEXT("[QuestLogWidget] Track Quest : %s"),*QuestInstance->QuestKey.ToString());

    // !!!!!!!!!!!!!!!! TODO:: Make Quest Tracker !!!!!!!!!!!!!!!!!!!!
}

void UQuestLogWidget::AddQuestEntryToScrollBox(UQuestInstance_Base* QuestInstance , UScrollBox* TargetBox)
{
    if (!QuestInstance || !TargetBox)
        return;

    UQuestEntryWidget* Entry =CreateWidget<UQuestEntryWidget>(GetOwningPlayer(), QuestEntryWidgetClass);
    Entry->InitWidget(QuestInstance);
    Entry->OnQuestSelected.AddUniqueDynamic(this, &ThisClass::HandleQuestEntrySelected);
    Entry->OnTrackSelected.AddUniqueDynamic( this, &ThisClass::HandleTrackSelected);

    TargetBox->AddChild(Entry);
}

void UQuestLogWidget::HideQuestDetail(bool bShown)
{
    WS_ShowQuestDetail->SetActiveWidgetIndex(bShown);
}
