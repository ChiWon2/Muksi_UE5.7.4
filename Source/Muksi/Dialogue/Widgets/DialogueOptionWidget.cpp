// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueOptionWidget.h"
#include"Components/TextBlock.h"
#include"Components/Button.h"

void UDialogueOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(BTN_OptionButton)
		BTN_OptionButton->OnClicked.AddDynamic(this, &UDialogueOptionWidget::HandleOptionButtonClicked);

}

void UDialogueOptionWidget::NativeDestruct()
{
	if (BTN_OptionButton)
	{
		BTN_OptionButton->OnClicked.RemoveAll(this);
	}
	Super::NativeDestruct();
}

void UDialogueOptionWidget::InitWidget(int32 OptionIdx, const FText& InText)
{
	OptionIndex = OptionIdx;
	if(TXT_OptionText)
		TXT_OptionText->SetText(InText);

}

void UDialogueOptionWidget::HandleOptionButtonClicked()
{
	OnOptionButtonClicked.Broadcast(OptionIndex);
}
