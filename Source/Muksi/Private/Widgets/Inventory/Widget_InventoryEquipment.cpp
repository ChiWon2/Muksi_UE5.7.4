#include "Widgets/Inventory/Widget_InventoryEquipment.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/StatComponent.h"
#include "GameFramework/Pawn.h"
#include "Items/Components/EquipmentComponent.h"
#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"
#include "Widgets/Inventory/Widget_EquipmentSlotEntry.h"
#include "Widgets/Inventory/Widget_InventoryItemEntry.h"

void UWidget_InventoryEquipment::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (RefreshButton)
	{
		RefreshButton->OnClicked.RemoveAll(this);
		RefreshButton->OnClicked.AddDynamic(this, &ThisClass::HandleRefreshClicked);
	}

	if (BackButton)
	{
		BackButton->OnClicked.RemoveAll(this);
		BackButton->OnClicked.AddDynamic(this, &ThisClass::HandleBackClicked);
		BackButton->SetFocus();
	}

	RefreshAll();
}

void UWidget_InventoryEquipment::HandleRefreshClicked()
{
	RefreshAll();
}

void UWidget_InventoryEquipment::HandleBackClicked()
{
	DeactivateWidget();
}

void UWidget_InventoryEquipment::HandleInventoryItemSelected(FGuid InstanceId)
{
	SelectedInstanceId = InstanceId;
	RefreshSelectedItemText();
}

void UWidget_InventoryEquipment::HandleInventoryItemEquipRequested(FGuid InstanceId)
{
	UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent || !InstanceId.IsValid())
	{
		return;
	}

	const bool bResult = EquipmentComponent->EquipItemByInstanceId(InstanceId);
	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Equip result: %s"), bResult ? TEXT("true") : TEXT("false"));

	RefreshAll();
}

void UWidget_InventoryEquipment::HandleEquipmentSlotUnequipRequested(EMuksiEquipmentSlot EquipmentSlot)
{
	UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();
	if (!EquipmentComponent || EquipmentSlot == EMuksiEquipmentSlot::None)
	{
		return;
	}

	const bool bResult = EquipmentComponent->UnequipItem(EquipmentSlot);
	UE_LOG(LogTemp, Log, TEXT("[InventoryEquipment] Unequip result: %s"), bResult ? TEXT("true") : TEXT("false"));

	RefreshAll();
}

void UWidget_InventoryEquipment::RefreshAll()
{
	RefreshInventoryList();
	RefreshEquipmentSlots();
	RefreshSelectedItemText();
	RefreshStatPreviewText();
}

void UWidget_InventoryEquipment::RefreshInventoryList()
{
	if (!InventoryListBox)
	{
		return;
	}

	InventoryListBox->ClearChildren();

	const UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();
	const UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();

	if (!InventoryComponent || !InventoryItemEntryClass)
	{
		return;
	}

	for (const FMuksiInventoryEntry& Entry : InventoryComponent->GetItems())
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		UWidget_InventoryItemEntry* EntryWidget = CreateWidget<UWidget_InventoryItemEntry>(GetOwningPlayer(), InventoryItemEntryClass);
		if (!EntryWidget)
		{
			continue;
		}

		const bool bEquipped = EquipmentComponent && EquipmentComponent->IsEquipped(Entry.InstanceId);

		EntryWidget->InitializeEntry(Entry, bEquipped);
		EntryWidget->OnSelected.AddDynamic(this, &ThisClass::HandleInventoryItemSelected);
		EntryWidget->OnEquipRequested.AddDynamic(this, &ThisClass::HandleInventoryItemEquipRequested);

		InventoryListBox->AddChild(EntryWidget);
	}
}

void UWidget_InventoryEquipment::RefreshEquipmentSlots()
{
	if (!EquipmentSlotListBox)
	{
		return;
	}

	EquipmentSlotListBox->ClearChildren();

	CreateEquipmentSlotEntry(EMuksiEquipmentSlot::Weapon);
	CreateEquipmentSlotEntry(EMuksiEquipmentSlot::Armor);
	CreateEquipmentSlotEntry(EMuksiEquipmentSlot::Accessory);
}

void UWidget_InventoryEquipment::CreateEquipmentSlotEntry(EMuksiEquipmentSlot EquipmentSlot)
{
	if (!EquipmentSlotListBox || !EquipmentSlotEntryClass)
	{
		return;
	}

	const UEquipmentComponent* EquipmentComponent = GetPlayerEquipmentComponent();

	FMuksiInventoryEntry EquippedEntry;
	const bool bHasEquippedEntry =
		EquipmentComponent &&
		EquipmentComponent->GetEquippedItem(EquipmentSlot, EquippedEntry) &&
		EquippedEntry.ItemData;

	UWidget_EquipmentSlotEntry* SlotWidget = CreateWidget<UWidget_EquipmentSlotEntry>(GetOwningPlayer(), EquipmentSlotEntryClass);
	if (!SlotWidget)
	{
		return;
	}

	SlotWidget->InitializeSlot(EquipmentSlot, EquippedEntry, bHasEquippedEntry);
	SlotWidget->OnUnequipRequested.AddDynamic(this, &ThisClass::HandleEquipmentSlotUnequipRequested);

	EquipmentSlotListBox->AddChild(SlotWidget);
}

void UWidget_InventoryEquipment::RefreshSelectedItemText()
{
	if (!SelectedItemText)
	{
		return;
	}

	if (!SelectedInstanceId.IsValid())
	{
		SelectedItemText->SetText(FText::FromString(TEXT("Selected: None")));
		return;
	}

	const UInventoryComponent* InventoryComponent = GetPlayerInventoryComponent();

	FMuksiInventoryEntry SelectedEntry;
	if (InventoryComponent && InventoryComponent->FindItemByInstanceId(SelectedInstanceId, SelectedEntry) && SelectedEntry.ItemData)
	{
		SelectedItemText->SetText(FText::FromString(
			FString::Printf(TEXT("Selected: %s"), *SelectedEntry.ItemData->DisplayName.ToString())
		));
		return;
	}

	SelectedItemText->SetText(FText::FromString(TEXT("Selected: Invalid")));
}

void UWidget_InventoryEquipment::RefreshStatPreviewText()
{
	if (!StatPreviewText)
	{
		return;
	}

	const UStatComponent* StatComponent = GetPlayerStatComponent();
	if (!StatComponent)
	{
		StatPreviewText->SetText(FText::FromString(TEXT("StatComponent: None")));
		return;
	}

	const FString Text = FString::Printf(
		TEXT("HP %.0f / %.0f\nAttack %.0f\nDefense %.0f\nMoveSpeed %.0f"),
		StatComponent->GetCurrentHP(),
		StatComponent->GetMaxHP(),
		StatComponent->GetAttackPower(),
		StatComponent->GetDefensePower(),
		StatComponent->GetMoveSpeed()
	);

	StatPreviewText->SetText(FText::FromString(Text));
}

UInventoryComponent* UWidget_InventoryEquipment::GetPlayerInventoryComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}

UEquipmentComponent* UWidget_InventoryEquipment::GetPlayerEquipmentComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UEquipmentComponent>() : nullptr;
}

UStatComponent* UWidget_InventoryEquipment::GetPlayerStatComponent() const
{
	const APlayerController* PC = GetOwningPlayer();
	const APawn* Pawn = PC ? PC->GetPawn() : nullptr;
	return Pawn ? Pawn->FindComponentByClass<UStatComponent>() : nullptr;
}