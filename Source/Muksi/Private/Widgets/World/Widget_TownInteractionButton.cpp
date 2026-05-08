#include "Widgets/World/Widget_TownInteractionButton.h"
#include "Components/Image.h"
#include "CommonTextBlock.h"

void UWidget_TownInteractionButton::InitializeInteraction(const FTownInteractionData& InInteractionData)
{
	InteractionData = InInteractionData;

	SetButtonText(InteractionData.DisplayName);

	if (DisplayNameText)
	{
		DisplayNameText->SetText(InteractionData.DisplayName);
	}


	if (DescriptionText)
	{
		DescriptionText->SetText(InteractionData.Description);
	}

	if (IconImage)
	{
		IconImage->SetBrushFromTexture(InteractionData.Icon);
	}

	SetIsEnabled(InteractionData.bUnlockedByDefault);
}

void UWidget_TownInteractionButton::NativeOnClicked()
{
	Super::NativeOnClicked();

	OnTownInteractionClicked.Broadcast(InteractionData);
}
