#include "Widgets/World/Widget_RepairItemEntry.h"

#include "Components/TextBlock.h"
#include "Input/Reply.h"
#include "Items/Data/MuksiItemDataAsset.h"

void UWidget_RepairItemEntry::InitializeEntry(const FMuksiInventoryEntry& InEntry, int32 InRepairCost)
{
	Entry = InEntry;
	RepairCost = InRepairCost;
	Refresh();
}

void UWidget_RepairItemEntry::SetSelected(bool bInSelected)
{
	bSelected = bInSelected;
	Refresh();
}

FReply UWidget_RepairItemEntry::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton && Entry.InstanceId.IsValid())
	{
		OnRightClicked.Broadcast(Entry.InstanceId);
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UWidget_RepairItemEntry::Refresh()
{
	const UMuksiItemDataAsset* ItemData = Entry.ItemData;

	if (ItemNameText)
	{
		ItemNameText->SetText(ItemData ? ItemData->DisplayName : FText::FromString(TEXT("None")));
	}

	if (EnhanceLevelText)
	{
		EnhanceLevelText->SetText(FText::FromString(FString::Printf(TEXT("+%d"), Entry.EnhanceLevel)));
	}

	if (DurabilityText)
	{
		const int32 DurabilityPercent = FMath::RoundToInt(FMath::Clamp(Entry.Durability, 0.f, 1.f) * 100.f);
		DurabilityText->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), DurabilityPercent)));
	}

	if (RepairCostText)
	{
		RepairCostText->SetText(FText::AsNumber(RepairCost));
	}
}