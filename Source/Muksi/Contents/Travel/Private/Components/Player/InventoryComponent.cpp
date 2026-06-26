#include "Muksi/Contents/Travel/Public/Components/Player/InventoryComponent.h"
#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(FName ItemID, UMuksiItemDataAsset* ItemData, int32 Quantity)
{
	if (ItemID.IsNone() || !ItemData || Quantity <= 0)
	{
		return false;
	}

	const int32 OldQuantity = GetItemCountByItemID(ItemID);
		
	if (ItemData->bStackable)
	{
		for (FMuksiInventoryEntry& Entry : Items)
		{
			if (Entry.ItemID == ItemID)
			{
				Entry.Quantity += Quantity;
				return true;
			}
		}

		FMuksiInventoryEntry NewEntry;
		NewEntry.ItemID = ItemID;
		NewEntry.InstanceId = FGuid::NewGuid();
		NewEntry.ItemData = ItemData;
		NewEntry.Quantity = Quantity;
		Items.Add(NewEntry);

		OnInventoryChanged.Broadcast(ItemID, OldQuantity, GetItemCountByItemID(ItemID));

		return true;
	}

	for (int32 Index = 0; Index < Quantity; ++Index)
	{
		FMuksiInventoryEntry NewEntry;
		NewEntry.ItemID = ItemID;
		NewEntry.InstanceId = FGuid::NewGuid();
		NewEntry.ItemData = ItemData;
		NewEntry.Quantity = 1;

		if (ItemData->ItemType == EMuksiItemType::Equipment)
		{
			NewEntry.Durability = 0.5f;
		}


		Items.Add(NewEntry);
	}

	OnInventoryChanged.Broadcast(ItemID, OldQuantity, GetItemCountByItemID(ItemID));

	return true;
}

bool UInventoryComponent::RemoveItemByInstanceId(FGuid InstanceId, int32 Quantity)
{
	if (!InstanceId.IsValid() || Quantity <= 0)
	{
		return false;
	}

	for (int32 Index = 0; Index < Items.Num(); ++Index)
	{
		FMuksiInventoryEntry& Entry = Items[Index];

		if (Entry.InstanceId != InstanceId)
		{
			continue;
		}
		if (!Entry.ItemData)
		{
			return false;
		}

		const FName ItemID = Entry.ItemID;
		const int32 OldQuantity = GetItemCountByItemID(ItemID);

		if (Entry.Quantity < Quantity)
		{
			return false;
		}

		Entry.Quantity -= Quantity;

		if (Entry.Quantity <= 0)
		{
			Items.RemoveAt(Index);
		}

		OnInventoryChanged.Broadcast(ItemID, OldQuantity, GetItemCountByItemID(ItemID));

		return true;
	}
	return false;
}

bool UInventoryComponent::HasItem(FName ItemID, int32 RequiredQuantity) const
{
	return GetItemCountByItemID(ItemID) >= RequiredQuantity;
}

int32 UInventoryComponent::GetItemCountByItemID(const FName& ItemID) const
{
	if (ItemID.IsNone())
	{
		return 0;
	}

	int32 TotalQuantity = 0;

	for (const FMuksiInventoryEntry& Entry : Items)
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		if (Entry.ItemID == ItemID)
		{
			TotalQuantity += Entry.Quantity;
		}
	}

	return TotalQuantity;
}

bool UInventoryComponent::FindItemByInstanceId(FGuid InstanceId, FMuksiInventoryEntry& OutEntry) const
{
	if (!InstanceId.IsValid())
	{
		return false;
	}

	for (const FMuksiInventoryEntry& Entry : Items)
	{
		if (Entry.InstanceId == InstanceId)
		{
			OutEntry = Entry;
			return true;
		}
	}

	return false;
}

FMuksiInventoryEntry* UInventoryComponent::FindMutableItemByInstanceId(FGuid InstanceId)
{
	if (!InstanceId.IsValid())
	{
		return nullptr;
	}

	for (FMuksiInventoryEntry& Entry : Items)
	{
		if (Entry.InstanceId == InstanceId)
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool UInventoryComponent::IsRepairableItem(FGuid InstanceId) const
{
	FMuksiInventoryEntry Entry;
	if (!FindItemByInstanceId(InstanceId, Entry))
	{
		return false;
	}

	if (!Entry.ItemData)
	{
		return false;
	}

	if (Entry.ItemData->ItemType != EMuksiItemType::Equipment)
	{
		return false;
	}

	const bool bRepairableSlot =
		Entry.ItemData->EquipmentSlot == EMuksiEquipmentSlot::Weapon ||
		Entry.ItemData->EquipmentSlot == EMuksiEquipmentSlot::Armor;

	if (!bRepairableSlot)
	{
		return false;
	}

	return Entry.Durability < 1.f;
}

int32 UInventoryComponent::GetRepairCost(FGuid InstanceId) const
{
	FMuksiInventoryEntry Entry;
	if (!FindItemByInstanceId(InstanceId, Entry))
	{
		return 0;
	}

	if (!IsRepairableItem(InstanceId) || !Entry.ItemData)
	{
		return 0;
	}

	const float ClampedDurability = FMath::Clamp(Entry.Durability, 0.f, 1.f);
	const float MissingRatio = 1.f - ClampedDurability;

	constexpr float RepairRate = 0.35f;
	const float EnhanceMultiplier = 1.f + static_cast<float>(Entry.EnhanceLevel) * 0.15f;

	const int32 RawCost = FMath::CeilToInt(
		static_cast<float>(Entry.ItemData->Price) * MissingRatio * RepairRate * EnhanceMultiplier
	);

	return FMath::Max(1, RawCost);
}

bool UInventoryComponent::RepairItem(FGuid InstanceId, int32& OutRepairCost)
{
	OutRepairCost = 0;

	FMuksiInventoryEntry* Entry = FindMutableItemByInstanceId(InstanceId);
	if (!Entry || !Entry->ItemData)
	{
		return false;
	}

	if (!IsRepairableItem(InstanceId))
	{
		return false;
	}

	OutRepairCost = GetRepairCost(InstanceId);
	if (OutRepairCost <= 0)
	{
		return false;
	}

	Entry->Durability = 1.f;
	return true;

}

namespace
{
	constexpr int32 MaxEnhanceLevel = 10;

	bool IsForgeEnhanceSlot(EMuksiEquipmentSlot Slot)
	{
		return Slot == EMuksiEquipmentSlot::Weapon ||
			Slot == EMuksiEquipmentSlot::Armor;
	}
}

bool UInventoryComponent::IsEnhanceableItem(FGuid InstanceId) const
{
	FMuksiInventoryEntry Entry;
	if (!FindItemByInstanceId(InstanceId, Entry))
	{
		return false;
	}

	if (!Entry.ItemData)
	{
		return false;
	}

	if (Entry.ItemData->ItemType != EMuksiItemType::Equipment)
	{
		return false;
	}

	if (!IsForgeEnhanceSlot(Entry.ItemData->EquipmentSlot))
	{
		return false;
	}

	if (Entry.EnhanceLevel >= MaxEnhanceLevel)
	{
		return false;
	}

	return Entry.Durability > 0.f;
}

int32 UInventoryComponent::GetEnhanceCost(FGuid InstanceId) const
{
	FMuksiInventoryEntry Entry;
	if (!FindItemByInstanceId(InstanceId, Entry))
	{
		return 0;
	}

	if (!IsEnhanceableItem(InstanceId) || !Entry.ItemData)
	{
		return 0;
	}

	const float BaseRate = 0.5f;
	const float LevelMultiplier = FMath::Pow(1.45f, static_cast<float>(Entry.EnhanceLevel));

	const int32 RawCost = FMath::CeilToInt(
		static_cast<float>(Entry.ItemData->Price) * BaseRate * LevelMultiplier
	);

	return FMath::Max(1, RawCost);
}

float UInventoryComponent::GetEnhanceSuccessRate(FGuid InstanceId) const
{
	FMuksiInventoryEntry Entry;
	if (!FindItemByInstanceId(InstanceId, Entry))
	{
		return 0.f;
	}

	if (!IsEnhanceableItem(InstanceId))
	{
		return 0.f;
	}

	const float RawRate = 0.95f - static_cast<float>(Entry.EnhanceLevel) * 0.05f;
	return FMath::Clamp(RawRate, 0.30f, 0.95f);
}

bool UInventoryComponent::EnhanceItem(
	FGuid InstanceId,
	int32& OutEnhanceCost,
	EMuksiEnhanceResult& OutResult)
{
	OutEnhanceCost = 0;
	OutResult = EMuksiEnhanceResult::None;

	FMuksiInventoryEntry* Entry = FindMutableItemByInstanceId(InstanceId);
	if (!Entry || !Entry->ItemData)
	{
		return false;
	}

	if (!IsEnhanceableItem(InstanceId))
	{
		return false;
	}

	OutEnhanceCost = GetEnhanceCost(InstanceId);
	if (OutEnhanceCost <= 0)
	{
		return false;
	}

	const float SuccessRate = GetEnhanceSuccessRate(InstanceId);
	const bool bSuccess = FMath::FRand() <= SuccessRate;

	if (bSuccess)
	{
		++Entry->EnhanceLevel;
		OutResult = EMuksiEnhanceResult::Success;
		return true;
	}

	if (Entry->EnhanceLevel >= 6)
	{
		Entry->EnhanceLevel = FMath::Max(0, Entry->EnhanceLevel - 1);
		Entry->Durability = FMath::Clamp(Entry->Durability - 0.20f, 0.f, 1.f);
		OutResult = EMuksiEnhanceResult::FailedDowngrade;
		return true;
	}

	Entry->Durability = FMath::Clamp(Entry->Durability - 0.15f, 0.f, 1.f);
	OutResult = EMuksiEnhanceResult::FailedNoChange;
	return true;
}

TArray<FMuksiInventoryEntry> UInventoryComponent::GetRepairableItems() const
{
	TArray<FMuksiInventoryEntry> RepairableItems;

	for (const FMuksiInventoryEntry& Entry : Items)
	{
		if (IsRepairableItem(Entry.InstanceId))
		{
			RepairableItems.Add(Entry);
		}
	}

	return RepairableItems;
}

TArray<FMuksiInventoryEntry> UInventoryComponent::GetForgeableEquipmentItems() const
{
	TArray<FMuksiInventoryEntry> ForgeableItems;

	for (const FMuksiInventoryEntry& Entry : Items)
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		if (Entry.ItemData->ItemType != EMuksiItemType::Equipment)
		{
			continue;
		}

		const bool bForgeableSlot =
			Entry.ItemData->EquipmentSlot == EMuksiEquipmentSlot::Weapon ||
			Entry.ItemData->EquipmentSlot == EMuksiEquipmentSlot::Armor;

		if (!bForgeableSlot)
		{
			continue;
		}

		ForgeableItems.Add(Entry);
	}

	return ForgeableItems;
}