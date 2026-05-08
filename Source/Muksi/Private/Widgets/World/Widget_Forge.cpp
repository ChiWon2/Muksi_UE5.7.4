#include "Widgets/World/Widget_Forge.h"
#include "Components/Button.h"

void UWidget_Forge::NativeOnActivated()
{
	Super::NativeOnActivated();

	UE_LOG(LogTemp, Log, TEXT("[Forge] NativeOnActivated"));

	if (RepairButton)
	{
		RepairButton->OnClicked.RemoveAll(this);
		RepairButton->OnClicked.AddDynamic(this, &UWidget_Forge::HandleRepairButtonClicked);
	}

	if (EnhanceButton)
	{
		EnhanceButton->OnClicked.RemoveAll(this);
		EnhanceButton->OnClicked.AddDynamic(this, &UWidget_Forge::HandleEnhanceButtonClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &UWidget_Forge::HandleBackButtonClicked);
		BackButton->SetFocus();
	}
}

void UWidget_Forge::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	UE_LOG(LogTemp, Log, TEXT("[Forge] NativeOnDeactivated"));
}

void UWidget_Forge::HandleRepairButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] Repair button clicked"));
	RepairWeapon();
}

void UWidget_Forge::HandleEnhanceButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] Enhance button clicked"));
	EnhanceWeapon();
}

void UWidget_Forge::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] Back button clicked"));
	DeactivateWidget();
}

void UWidget_Forge::RepairWeapon()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] RepairWeapon entry point"));
	// TODO: Connect equipment durability repair system later.
}

void UWidget_Forge::EnhanceWeapon()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] EnhanceWeapon entry point"));
	// TODO: Connect weapon enhancement system later.
}
