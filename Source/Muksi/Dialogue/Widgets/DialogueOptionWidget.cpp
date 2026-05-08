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

void UDialogueOptionWidget::InitWidget(const FDialogueOption& InOption, int32 InOptionIndex, bool bInIsEnabled)
{
	OptionIndex = InOptionIndex;

	if (TXT_OptionText)
	{
		TXT_OptionText->SetText(InOption.OptionText);
	}

	if (BTN_OptionButton)
	{
		BTN_OptionButton->SetIsEnabled(bInIsEnabled);
	}

	UpdateVisual(bInIsEnabled);
}

void UDialogueOptionWidget::UpdateVisual(bool bInIsEnabled)
{
	if (!TXT_OptionText)
		return;

	if (bInIsEnabled)
	{
		TXT_OptionText->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	else
	{
		TXT_OptionText->SetColorAndOpacity(FSlateColor(FLinearColor::Gray));
	}
}

void UDialogueOptionWidget::HandleOptionButtonClicked()
{
	if (BTN_OptionButton && !BTN_OptionButton->GetIsEnabled())
		return;

	OnOptionButtonClicked.Broadcast(OptionIndex);
}