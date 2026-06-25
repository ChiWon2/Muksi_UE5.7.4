#include "QuestLogWidget.h"

#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"

#include "Kismet/GameplayStatics.h"

#include "../Quest/QuestDetailRow.h"
#include "../Quest/QuestInstance_Base.h"
#include "../QuestSubsystem.h"
#include"../TravelTime/TravelTimeSubsystem.h"
#include "TabButton.h"
#include "QuestEntryWidget.h"
#include "QuestObjectiveEntryWidget.h"



void UQuestLogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    TAB_Ongoing->SetTabText(FText::FromString("Quests"));
    TAB_Completed->SetTabText(FText::FromString("Completed"));
    TAB_Ongoing->SetSelected(true);
    TAB_Completed->SetSelected(false);

    WS_QuestList->SetActiveWidgetIndex(0);

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
    UTravelTimeSubsystem::Get(this)->StopTravelTime();
    RefreshQuestList();
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

void UQuestLogWidget::RefreshQuestList()
{
    UQuestSubsystem* QuestSubsystem = UQuestSubsystem::Get(this);

    if (!QuestSubsystem)
        return;

    SB_ActiveQuests->ClearChildren();
    SB_CompletedQuests->ClearChildren();
    

    for (const auto& Pair : QuestSubsystem->GetActiveQuests())
    {
        AddQuestToList(Pair.Value, SB_ActiveQuests);
    }
    for (const auto& Pair : QuestSubsystem->GetCompletedQuests())
    {
        AddQuestToList(Pair.Value, SB_CompletedQuests);
    }
}

void UQuestLogWidget::HandleTabClicked(UTabButton* ClickedTab)
{
    TAB_Ongoing->SetSelected(false);
    TAB_Completed->SetSelected(false);

    ClickedTab->SetSelected(true);

    if (ClickedTab == TAB_Ongoing)
    {
        WS_QuestList->SetActiveWidgetIndex(0);
    }
    else
    {
        WS_QuestList->SetActiveWidgetIndex(1);
    }
}

void UQuestLogWidget::HandleQuestSelected(UQuestInstance_Base* QuestInstance)
{
    RefreshQuestDetails(QuestInstance);
}

void UQuestLogWidget::RefreshQuestDetails(UQuestInstance_Base* QuestInstance)
{
    if (!QuestInstance)
        return;

    SelectedQuestInstance = QuestInstance;

    if (VB_Objectives)
    {
        VB_Objectives->ClearChildren();
    }

    if (WS_ShowQuestDetail)
    {
        WS_ShowQuestDetail->SetActiveWidgetIndex(1);
    }

    const FQuestDetailRow& QuestDetails = QuestInstance->QuestDetails;

    if (TXT_QuestName)
    {
        TXT_QuestName->SetText(QuestDetails.QuestName);
    }

    if (TXT_QuestDescription)
    {
        TXT_QuestDescription->SetText(QuestDetails.Description);
    }

    APlayerController* PC = GetOwningPlayer();

    for (const FObjectiveDetails& Objective : QuestDetails.Objectives)
    {
        UQuestObjectiveEntryWidget* Entry = CreateWidget<UQuestObjectiveEntryWidget>( PC, QuestObjectiveEntryWidgetClass);

        if (!Entry)
            continue;

        Entry->InitWidget( Objective, QuestInstance);

        VB_Objectives->AddChild(Entry);
    }
}

void UQuestLogWidget::HandleTrackSelected(
    UQuestInstance_Base* QuestInstance)
{
    if (!QuestInstance)
        return;

    UE_LOG( LogTemp, Warning, TEXT("[QuestLogWidget] Track Quest : %s"),*QuestInstance->QuestKey.ToString());

    // TODO:
    // Make Quest Tracker 
}

void UQuestLogWidget::AddQuestToList( UQuestInstance_Base* QuestInstance,UScrollBox* TargetBox)
{
    if (!QuestInstance || !TargetBox)
        return;

    APlayerController* PC = GetOwningPlayer();

    UQuestEntryWidget* Entry =CreateWidget<UQuestEntryWidget>( PC, QuestEntryWidgetClass);

    if (!Entry)
        return;

    Entry->InitWidget(QuestInstance);

    Entry->OnQuestSelected.AddDynamic(this, &ThisClass::HandleQuestSelected);

    Entry->OnTrackSelected.AddDynamic( this, &ThisClass::HandleTrackSelected);

    TargetBox->AddChild(Entry);
}