#include "Widgets/World/Widget_Forge.h"

#include "Items/Components/InventoryComponent.h"
#include "Items/Components/EquipmentComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Widgets/Inventory/Widget_ItemSlot.h"
#include "Widgets/World/Widget_ForgeConfirmPopup.h"

#include "Controllers/MuksiPlayerController.h"
#include "MuksiGameplayTags.h"
#include "Subsystems/MuksiUISubsystem.h"
#include "Components/StatComponent.h"

#include "GameFramework/Pawn.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"

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

	RefreshForgeItemList();
}

void UWidget_Forge::NativeOnDeactivated()
{
	SelectedForgeItemInstanceId.Invalidate();

	Super::NativeOnDeactivated();

	UE_LOG(LogTemp, Log, TEXT("[Forge] NativeOnDeactivated"));
}

void UWidget_Forge::HandleRepairButtonClicked()
{
	if (!SelectedForgeItemInstanceId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Forge] No item selected for repair"));
		return;
	}

	PushForgeConfirmPopup(SelectedForgeItemInstanceId, EMuksiForgeActionType::Repair);
}

void UWidget_Forge::HandleEnhanceButtonClicked()
{
	if (!SelectedForgeItemInstanceId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Forge] No item selected for enhance"));
		return;
	}

	PushForgeConfirmPopup(SelectedForgeItemInstanceId, EMuksiForgeActionType::Enhance);
}

void UWidget_Forge::HandleBackButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] Back button clicked"));
	DeactivateWidget();
}

void UWidget_Forge::HandleForgeItemClicked(FGuid InstanceId)
{
	UE_LOG(LogTemp, Log, TEXT("[Forge] Item clicked. InstanceId=%s"),
		*InstanceId.ToString(EGuidFormats::DigitsWithHyphens));

	if (!InstanceId.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Forge] Item clicked but InstanceId is invalid"));
		return;
	}

	SelectedForgeItemInstanceId = InstanceId;
	RefreshForgeItemList();
}

void UWidget_Forge::HandleForgeActionCompleted(
	FGuid InstanceId,
	EMuksiForgeActionType ActionType)
{
	if (ActionType == EMuksiForgeActionType::Enhance)
	{
		const APlayerController* PC = GetOwningPlayer();
		APawn* Pawn = PC ? PC->GetPawn() : nullptr;

		const UEquipmentComponent* EquipmentComponent = Pawn ? Pawn->FindComponentByClass<UEquipmentComponent>() : nullptr;
		UStatComponent* StatComponent = Pawn ? Pawn->FindComponentByClass<UStatComponent>() : nullptr;

		if (EquipmentComponent && StatComponent && EquipmentComponent->IsEquipped(InstanceId))
		{
			StatComponent->RefreshStats();
		}
	}

	RefreshForgeItemList();
}

void UWidget_Forge::RefreshForgeItemList()
{
	if (!ForgeItemGridPanel)
	{
		return;
	}

	ForgeItemGridPanel->ClearChildren();

	const UInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (!InventoryComponent || !ForgeItemSlotClass)
	{
		return;
	}

	const int32 SafeColumnCount = FMath::Max(1, ForgeItemColumnCount);
	int32 VisibleIndex = 0;

	for (const FMuksiInventoryEntry& Entry : InventoryComponent->GetForgeableEquipmentItems())
	{
		UWidget_ItemSlot* SlotWidget = CreateWidget<UWidget_ItemSlot>(GetOwningPlayer(), ForgeItemSlotClass);
		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->InitializeSlot(Entry, false);
		SlotWidget->SetSelected(Entry.InstanceId == SelectedForgeItemInstanceId);
		SlotWidget->OnClicked.AddDynamic(this, &ThisClass::HandleForgeItemClicked);

		const int32 Row = VisibleIndex / SafeColumnCount;
		const int32 Column = VisibleIndex % SafeColumnCount;

		ForgeItemGridPanel->AddChildToUniformGrid(SlotWidget, Row, Column);

		++VisibleIndex;
	}
}

void UWidget_Forge::PushForgeConfirmPopup(
	FGuid InstanceId,
	EMuksiForgeActionType ActionType)
{
	if (!InstanceId.IsValid() || ForgeConfirmPopupClass.IsNull())
	{
		return;
	}

	AMuksiPlayerController* PC = GetOwningMuksiPlayerController();
	UMuksiUISubsystem* UISubsystem = UMuksiUISubsystem::Get(this);

	if (!PC || !UISubsystem)
	{
		return;
	}

	UISubsystem->PushSoftWidgetToStackAsync(
		PC,
		MuksiGameplayTag::Muksi_WidgetStack_Modal,
		ForgeConfirmPopupClass,
		true,
		[this, InstanceId, ActionType](UWidget_ActivatableBase* CreatedWidget)
		{
			UWidget_ForgeConfirmPopup* Popup = Cast<UWidget_ForgeConfirmPopup>(CreatedWidget);
			if (!Popup)
			{
				return;
			}

			Popup->InitializeForgePopup(InstanceId, ActionType);
			Popup->OnForgeActionCompleted.AddUObject(this, &ThisClass::HandleForgeActionCompleted);
		}
	);
}

UInventoryComponent* UWidget_Forge::GetInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}
