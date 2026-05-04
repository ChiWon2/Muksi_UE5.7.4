// DialogueWidget.cpp

#include "DialogueWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/RichTextBlock.h"

#include "../DialogueSubsystem.h"
#include "../TravelTime/TravelTimeSubsystem.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DialogueSubSystem = UDialogueSubsystem::Get(this);

	InitWidget();
}

void UDialogueWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UDialogueWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	BindToSubsystem();
}

void UDialogueWidget::NativeOnDeactivated()
{
	UnbindFromSubsystem();

	Super::NativeOnDeactivated();
}

void UDialogueWidget::InitWidget()
{
	if (VB_Options)
	{
		VB_Options->ClearChildren();
	}
}

void UDialogueWidget::BindToSubsystem()
{
	if (!DialogueSubSystem)
	{
		return;
	}

	DialogueSubSystem->OnDialogueTextUpdated.AddDynamic(this,&UDialogueWidget::OnDialogueTextUpdated);

	DialogueSubSystem->OnDialogueOptionsUpdated.AddDynamic(this,&UDialogueWidget::OnDialogueOptionsUpdated);

	DialogueSubSystem->OnDialogueEnded.AddDynamic(this,&UDialogueWidget::OnDialogueEnded);
}

void UDialogueWidget::UnbindFromSubsystem()
{
	if (!DialogueSubSystem)
		return;

	DialogueSubSystem->OnDialogueTextUpdated.RemoveDynamic(this,&UDialogueWidget::OnDialogueTextUpdated);

	DialogueSubSystem->OnDialogueOptionsUpdated.RemoveDynamic(this,&UDialogueWidget::OnDialogueOptionsUpdated);

	DialogueSubSystem->OnDialogueEnded.RemoveDynamic(this,&UDialogueWidget::OnDialogueEnded);
}

void UDialogueWidget::OnDialogueTextUpdated(const FText& NewText)
{
	if (TXT_DialogueText)
	{
		TXT_DialogueText->SetText(NewText);
	}
}

void UDialogueWidget::OnDialogueOptionsUpdated(const TArray<FDialogueOption>& Options)
{
	if (!VB_Options)
	{
		return;
	}

	VB_Options->ClearChildren();

	for (int32 i = 0; i < Options.Num(); ++i)
	{
		if (!DialogueOptionWidgetClass)
		{
			continue;
		}

		UDialogueOptionWidget* OptionWidget =CreateWidget<UDialogueOptionWidget>(this,DialogueOptionWidgetClass);

		if (!OptionWidget)
		{
			continue;
		}

		OptionWidget->InitWidget(Options[i], i);

		OptionWidget->OnOptionButtonClicked.AddDynamic(this,&UDialogueWidget::HandleOptionButtonClicked);

		VB_Options->AddChild(OptionWidget);
	}
}

void UDialogueWidget::OnDialogueEnded()
{
	DeactivateWidget();
}

void UDialogueWidget::HandleOptionButtonClicked(int32 OptionIndex)
{
	DialogueSubSystem->SelectOption(OptionIndex);
}