#include "Widgets/Inventory/Widget_EquipmentSlotEntry.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_EquipmentSlotEntry::NativeConstruct()
{
	Super::NativeConstruct();

	if (UnequipButton)
	{
		UnequipButton->OnClicked.RemoveAll(this);
		UnequipButton->OnClicked.AddDynamic(this, &ThisClass::HandleUnequipClicked);
	}

	Refresh();
}

void UWidget_EquipmentSlotEntry::InitializeSlot(EMuksiEquipmentSlot InSlot, const FMuksiInventoryEntry& InEquippedEntry, bool bInHasEquippedEntry)
{
	Slot = InSlot;
	EquippedEntry = InEquippedEntry;
	bHasEquippedEntry = bInHasEquippedEntry;

	Refresh();
}

void UWidget_EquipmentSlotEntry::HandleUnequipClicked()
{
	if (Slot != EMuksiEquipmentSlot::None)
	{
		OnUnequipRequested.Broadcast(Slot);
	}
}

void UWidget_EquipmentSlotEntry::Refresh()
{
	if (SlotNameText)
	{
		const UEnum* SlotEnum = StaticEnum<EMuksiEquipmentSlot>();
		const FText SlotText = SlotEnum
			? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(Slot))
			: FText::FromString(TEXT("None"));

		SlotNameText->SetText(SlotText);
	}

	if (EquippedItemNameText)
	{
		const FText ItemText =
			bHasEquippedEntry && EquippedEntry.ItemData
			? EquippedEntry.ItemData->DisplayName
			: FText::FromString(TEXT("None"));

		EquippedItemNameText->SetText(ItemText);
	}

	if (UnequipButton)
	{
		UnequipButton->SetIsEnabled(bHasEquippedEntry);
	}
}