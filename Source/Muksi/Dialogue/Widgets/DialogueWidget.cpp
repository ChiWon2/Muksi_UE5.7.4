// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueWidget.h"
#include"Components/VerticalBox.h"
#include"Components/TextBlock.h"
#include"../DialogueOption.h"
#include"../DialogueSubsystem.h"
#include"Components/RichTextBlock.h"
#include"../../TravelTime/TravelTimeSubsystem.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BindToSubsystem();
}

void UDialogueWidget::NativeDestruct()
{
	if (DialogueSubSystem)
	{
		DialogueSubSystem->OnDialogueTextUpdated.RemoveAll(this);
		DialogueSubSystem->OnDialogueOptionsUpdated.RemoveAll(this);
		DialogueSubSystem->OnDialogueEnded.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UDialogueWidget::OnDialogueTextUpdated(const FText& NewText)
{
	TXT_DialogueText->SetText(NewText);
	StopTravelTime();
}

void UDialogueWidget::OnDialogueOptionsUpdated(const TArray<FDialogueOption>& Options)
{
	VB_Options->ClearChildren();
	for (int32 i = 0; i < Options.Num(); ++i)
	{
		UDialogueOptionWidget* OptionWidget = CreateWidget<UDialogueOptionWidget>(this, DialogueOptionWidgetClass);
		OptionWidget->InitWidget(i, Options[i].OptionText);
		OptionWidget->OnOptionButtonClicked.AddDynamic(this, &UDialogueWidget::HandleOptionButtonClicked);
		VB_Options->AddChild(OptionWidget);
	}
}

void UDialogueWidget::OnDialogueEnded()
{
	StartTravelTime();

	RemoveFromParent();
}

void UDialogueWidget::HandleOptionButtonClicked(int32 OptionIndex)
{
	DialogueSubSystem->SelectOption(OptionIndex);
}

void UDialogueWidget::InitWidget()
{

}

void UDialogueWidget::BindToSubsystem()
{
	DialogueSubSystem = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
	if (DialogueSubSystem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UDialogueWidget] There is No DialogueSubSystem, Check this->BindToDialogueSubsystem Called"));
		return;
	}
	if (DialogueSubSystem->GetDialogueTable() == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("[UDialogueWidget] There is No DialogueTable in SubSystem, Check DialogueQuest GameInstance and SubSystem->SetDataTable Called"));
		return;
	}
	DialogueSubSystem->OnDialogueTextUpdated.AddDynamic(this, &UDialogueWidget::OnDialogueTextUpdated);
	DialogueSubSystem->OnDialogueOptionsUpdated.AddDynamic(this, &UDialogueWidget::OnDialogueOptionsUpdated);
	DialogueSubSystem->OnDialogueEnded.AddDynamic(this, &UDialogueWidget::OnDialogueEnded);

	TravelTimeSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UTravelTimeSubsystem>();
}
void UDialogueWidget::StartTravelTime()
{
	TravelTimeSubsystem->StartTravelTime();
}
void UDialogueWidget::StopTravelTime()
{
	TravelTimeSubsystem->StopTravelTime();
}