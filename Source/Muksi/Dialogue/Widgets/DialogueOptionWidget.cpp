// DialogueOptionWidget.cpp

#include "DialogueOptionWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UDialogueOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	check(BTN_OptionButton);

	BTN_OptionButton->OnClicked.AddDynamic(this,&UDialogueOptionWidget::HandleOptionButtonClicked);
}

void UDialogueOptionWidget::NativeDestruct()
{
	if (BTN_OptionButton)
	{
		BTN_OptionButton->OnClicked.RemoveAll(this);
	}

	Super::NativeDestruct();
}

void UDialogueOptionWidget::InitWidget(const FDialogueOption& InOption ,int32 InOptionIndex)
{
	OptionIndex = InOptionIndex;

	if (TXT_OptionText)
	{
		TXT_OptionText->SetText(InOption.OptionText);
	}
}

void UDialogueOptionWidget::HandleOptionButtonClicked()
{
	OnOptionButtonClicked.Broadcast(OptionIndex);
}