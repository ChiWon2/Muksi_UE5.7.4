#include "Widgets/Inventory/Widget_EquipmentSlot.h"

#include "Input/Reply.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_EquipmentSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SlotButton)
	{
		SlotButton->OnClicked.RemoveAll(this);
		SlotButton->OnClicked.AddDynamic(this, &ThisClass::HandleSlotClicked);
	}

	Refresh();
}

void UWidget_EquipmentSlot::InitializeSlot(EMuksiEquipmentSlot InSlot, const FMuksiInventoryEntry& InEquippedEntry, bool bInHasEquippedItem)
{
	Slot = InSlot;
	EquippedEntry = InEquippedEntry;
	bHasEquippedItem = bInHasEquippedItem;
	Refresh();
}

void UWidget_EquipmentSlot::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	Refresh();
}

void UWidget_EquipmentSlot::HandleSlotClicked()
{
	if (Slot != EMuksiEquipmentSlot::None)
	{
		OnClicked.Broadcast(Slot);
	}
}

void UWidget_EquipmentSlot::Refresh()
{
	if (SlotNameText)
	{
		const UEnum* SlotEnum = StaticEnum<EMuksiEquipmentSlot>();
		SlotNameText->SetText(SlotEnum ? SlotEnum->GetDisplayNameTextByValue(static_cast<int64>(Slot)) : FText::GetEmpty());
	}

	const UMuksiItemDataAsset* ItemData = bHasEquippedItem ? EquippedEntry.ItemData : nullptr;

	if (ItemIconImage)
	{
		ItemIconImage->SetVisibility(ItemData && ItemData->Icon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);

		if (ItemData && ItemData->Icon)
		{
			ItemIconImage->SetBrushFromTexture(ItemData->Icon);
		}
	}

	if (EmptyText)
	{
		EmptyText->SetVisibility(ItemData ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	}
}

FReply UWidget_EquipmentSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (Slot != EMuksiEquipmentSlot::None && bHasEquippedItem)
		{
			OnRightClicked.Broadcast(Slot);
			return FReply::Handled();
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}