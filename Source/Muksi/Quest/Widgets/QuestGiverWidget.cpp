// Fill out your copyright notice in the Description page of Project Settings.

#include "QuestGiverWidget.h"
#include"Kismet/GameplayStatics.h"
#include"GameFramework/Character.h"
#include"../QuestSubsystem.h"
#include"../DeveloperSettings/QuestDeveloperSettings.h"
#include"GameFramework/PlayerController.h"
#include"QuestObjectiveEntryWidget.h"
#include"../QuestInstance_Base.h"
#include"Components/Button.h"
#include"Components/TextBlock.h"
#include"Components/VerticalBox.h"

void UQuestGiverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	VB_Objectives->ClearChildren();

	BTN_Accept->OnClicked.AddUniqueDynamic(this, &UQuestGiverWidget::OnAcceptButtonClicked);
	BTN_Decline->OnClicked.AddUniqueDynamic(this, &UQuestGiverWidget::OnDeclineButtonClicked);

	APlayerController* PC = GetOwningPlayer();
	const UQuestDeveloperSettings* Settings = GetDefault<UQuestDeveloperSettings>();

	for (FObjectiveDetails Objective : Details.Objectives)
	{
		UQuestObjectiveEntryWidget* Entry = CreateWidget<UQuestObjectiveEntryWidget>(PC, Settings->QuestObjectiveEntryWidgetClass);

		Entry->InitWidget(Objective);
		VB_Objectives->AddChild(Entry);
	}
}

void UQuestGiverWidget::NativeDestruct()
{
	BTN_Accept->OnClicked.RemoveDynamic(this, &UQuestGiverWidget::OnAcceptButtonClicked);
	BTN_Decline->OnClicked.RemoveDynamic(this, &UQuestGiverWidget::OnDeclineButtonClicked);

	Super::NativeDestruct();
}

void UQuestGiverWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
}

void UQuestGiverWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

void UQuestGiverWidget::InitWidget(const FQuestKey& InQuestKey)
{
	QuestKey = InQuestKey;

	Details = *UQuestSubsystem::Get(this)->GetQuestRow(QuestKey);

	if (TXT_QuestName)
	{
		TXT_QuestName->SetText(Details.QuestName);
	}

	if (TXT_QuestDescription)
	{
		TXT_QuestDescription->SetText(Details.Description);
	}
}

void UQuestGiverWidget::OnAcceptButtonClicked()
{
	//GetPlayerCharacter -> GetComponent By Class(QuestLogComponent) -> is Valid -> QuestLogComponent->AddNewQuest(QuestID); -> RemoveFromParent;
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	UQuestSubsystem::Get(this)->AddNewQuest(QuestKey);

	DeactivateWidget();
}

void UQuestGiverWidget::OnDeclineButtonClicked()
{
	DeactivateWidget();
}
