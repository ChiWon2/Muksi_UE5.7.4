#include "Widgets/Inventory/Widget_InventoryItemEntry.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_InventoryItemEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveAll(this);
		SelectButton->OnClicked.AddDynamic(this, &ThisClass::HandleSelectClicked);
	}

	if (EquipButton)
	{
		EquipButton->OnClicked.RemoveAll(this);
		EquipButton->OnClicked.AddDynamic(this, &ThisClass::HandleEquipClicked);
	}

	Refresh();
}

void UWidget_InventoryItemEntry::InitializeEntry(const FMuksiInventoryEntry& InEntry, bool bInEquipped)
{
	Entry = InEntry;
	bEquipped = bInEquipped;

	Refresh();
}

void UWidget_InventoryItemEntry::HandleSelectClicked()
{
	if (Entry.InstanceId.IsValid())
	{
		OnSelected.Broadcast(Entry.InstanceId);
	}
}

void UWidget_InventoryItemEntry::HandleEquipClicked()
{
	if (Entry.InstanceId.IsValid())
	{
		OnEquipRequested.Broadcast(Entry.InstanceId);
	}
}

void UWidget_InventoryItemEntry::Refresh()
{
	if (ItemNameText)
	{
		ItemNameText->SetText(Entry.ItemData ? Entry.ItemData->DisplayName : FText::FromString(TEXT("None")));
	}

	if (QuantityText)
	{
		QuantityText->SetText(FText::FromString(FString::Printf(TEXT("x%d"), Entry.Quantity)));
	}

	if (EquippedText)
	{
		EquippedText->SetText(bEquipped ? FText::FromString(TEXT("Equipped")) : FText::GetEmpty());
	}

	if (EquipButton)
	{
		const bool bCanEquip =
			Entry.ItemData &&
			Entry.ItemData->ItemType == EMuksiItemType::Equipment &&
			Entry.ItemData->EquipmentSlot != EMuksiEquipmentSlot::None &&
			!bEquipped;

		EquipButton->SetIsEnabled(bCanEquip);
	}
}