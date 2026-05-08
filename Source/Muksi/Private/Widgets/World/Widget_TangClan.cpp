#include "Widgets/World/Widget_TangClan.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UWidget_TangClan::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("[TangClan] Opened"));

	if (MessageText)
	{
		MessageText->SetText(FText::FromString(TEXT("TangClan Subquest Text.")));
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UWidget_TangClan::HandleBackButtonClicked);
		BackButton->SetFocus();
	}
}

void UWidget_TangClan::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[TangClan] Back button clicked"));

	DeactivateWidget();
}
