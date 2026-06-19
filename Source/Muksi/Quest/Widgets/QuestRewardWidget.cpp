// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestRewardWidget.h"
#include"Kismet/GameplayStatics.h"
#include"GameFramework/Character.h"
#include"../QuestSubsystem.h"
#include"GameFramework/PlayerController.h"
#include"QuestObjectiveEntryWidget.h"
#include"../QuestInstance_Base.h"
#include"Components/Button.h"
#include"Components/TextBlock.h"
#include"Components/VerticalBox.h"

void UQuestRewardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BTN_ClearQuest)
		BTN_ClearQuest->OnClicked.AddUniqueDynamic(this, &UQuestRewardWidget::OnClearQuestButtonClicked);
	if (BTN_Cancel)
		BTN_Cancel->OnClicked.AddUniqueDynamic(this, &UQuestRewardWidget::OnCancelButtonClicked);

	APlayerController* PC = GetOwningPlayer();

	for (FObjectiveDetails Objective : Details.Objectives)
	{
		UQuestObjectiveEntryWidget* Entry = CreateWidget<UQuestObjectiveEntryWidget>(PC, QuestObjectiveEntryWidgetClass);
		Entry->InitWidget(Objective, CurrentQuestInstance);
		VB_Objectives->AddChild(Entry);
	}
}

void UQuestRewardWidget::NativeDestruct()
{
	BTN_ClearQuest->OnClicked.RemoveDynamic(this, &UQuestRewardWidget::OnClearQuestButtonClicked);
	BTN_Cancel->OnClicked.RemoveDynamic(this, &UQuestRewardWidget::OnCancelButtonClicked);

	Super::NativeDestruct();
}

void UQuestRewardWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UQuestRewardWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UQuestRewardWidget::InitWidget(const FQuestKey& InQuestKey)
{
	QuestKey = InQuestKey;
	
	Details = *UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);
	
	CurrentQuestInstance = UQuestSubsystem::Get(this)->GetActiveQuestInstance(QuestKey);

	if (TXT_QuestName)
	{
		TXT_QuestName->SetText(Details.QuestName);
	}

	if (TXT_QuestDescription)
	{
		TXT_QuestDescription->SetText(Details.Description);
	}
}

void UQuestRewardWidget::OnClearQuestButtonClicked()
{
	//GetPlayerCharacter -> GetComponent By Class(QuestLogComponent) -> is Valid -> QuestLogComponent->AddNewQuest(QuestID); -> RemoveFromParent;

	UQuestSubsystem::Get(this)->CompleteQuest(QuestKey);
	DeactivateWidget();
}

void UQuestRewardWidget::OnCancelButtonClicked()
{
	DeactivateWidget();
}
