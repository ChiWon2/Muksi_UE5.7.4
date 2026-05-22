#include "World/Components/InventoryEquipmentUIComponent.h"

#include "Controllers/MuksiPlayerController.h"
#include "MuksiFunctionLibrary.h"
#include "MuksiGameplayTags.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "Widgets/Widget_ActivatableBase.h"

UInventoryEquipmentUIComponent::UInventoryEquipmentUIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryEquipmentUIComponent::OpenInventoryEquipmentUI()
{
	if (CurrentInventoryEquipmentWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Open skipped: already open"));
		return;
	}

	AMuksiPlayerController* PC = Cast<AMuksiPlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryEquipment] Open failed: owner is not MuksiPlayerController"));
		return;
	}

	if (const UMuksiPlayerDataSubsystem* PlayerData = UMuksiPlayerDataSubsystem::Get(PC))
	{
		UE_LOG(LogTemp, Warning, TEXT("Subsystem OK"));
	}

	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(PC);
	if (!UISubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryEquipment] Open failed: UISubsystem is null"));
		return;
	}

	TSoftClassPtr<UWidget_ActivatableBase> ResolvedWidgetClass = InventoryEquipmentWidgetClass;
	if (ResolvedWidgetClass.IsNull())
	{
		ResolvedWidgetClass = UMuksiFunctionLibrary::GetMuksiSoftWidgetClassByTag(
			MuksiGameplayTag::Muksi_Widget_World_InventoryEquipment
		);
	}

	if (ResolvedWidgetClass.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[InventoryEquipment] Open failed: WidgetClass is null"));
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_GameHud,
		ResolvedWidgetClass,
		true,
		[this](UWidget_ActivatableBase* CreatedWidget)
		{
			if (!CreatedWidget)
			{
				return;
			}

			CurrentInventoryEquipmentWidget = CreatedWidget;

			UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] CreatedWidget=%s"), *GetNameSafe(CreatedWidget));

			CreatedWidget->OnDeactivated().AddWeakLambda(this, [this]()
				{
					UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Widget deactivated. Clear current widget."));
					CurrentInventoryEquipmentWidget = nullptr;
				});
		},
		[](UWidget_ActivatableBase* PushedWidget)
		{
			UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] PushedWidget=%s"), *GetNameSafe(PushedWidget));
		}
	);
}

void UInventoryEquipmentUIComponent::CloseInventoryEquipmentUI()
{
	if (!CurrentInventoryEquipmentWidget)
	{
		UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Close skipped: not open"));
		return;
	}

	UWidget_ActivatableBase* WidgetToClose = CurrentInventoryEquipmentWidget;
	CurrentInventoryEquipmentWidget = nullptr;

	WidgetToClose->DeactivateWidget();

	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Closed"));
}