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
#include "QuestEntryWidget.h"
#include "QuestObjectiveEntryWidget.h"

void UQuestLogWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (BT_CloseMenu)
    {
        BT_CloseMenu->OnClicked.AddDynamic(this, &UQuestLogWidget::OnCloseButtonClicked);
    }
}

void UQuestLogWidget::NativeDestruct()
{
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

    if (SB_MainQuest)
    {
        SB_MainQuest->ClearChildren();
    }

    if (SB_SideQuest)
    {
        SB_SideQuest->ClearChildren();
    }

    APlayerController* PC = GetOwningPlayer();

    for (const TPair<FQuestKey, TObjectPtr<UQuestInstance_Base>>&Pair : QuestSubsystem->GetActiveQuests())
    {
        UQuestInstance_Base* QuestInstance = Pair.Value;

        if (!QuestInstance)
            continue;

        UQuestEntryWidget* Entry = CreateWidget<UQuestEntryWidget>(PC, QuestEntryWidgetClass);

        if (!Entry)
            continue;

        Entry->InitWidget(QuestInstance);

        Entry->OnQuestSelected.AddDynamic( this, &UQuestLogWidget::HandleQuestSelected);

        Entry->OnTrackSelected.AddDynamic( this, &UQuestLogWidget::HandleTrackSelected);

        const bool bIsCompletedQuest = QuestInstance->bIsCompleted;

        UScrollBox* TargetBox = bIsCompletedQuest ? SB_MainQuest : SB_SideQuest;

        if (TargetBox)
        {
            TargetBox->AddChild(Entry);
        }
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