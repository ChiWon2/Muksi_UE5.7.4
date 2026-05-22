#include "Items/Components/InventoryComponent.h"
#include "Items/Data/MuksiItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(UMuksiItemDataAsset* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0)
	{
		return false;
	}

	if (ItemData->bStackable)
	{
		for (FMuksiInventoryEntry& Entry : Items)
		{
			if (Entry.ItemData == ItemData)
			{
				Entry.Quantity += Quantity;
				return true;
			}
		}

		FMuksiInventoryEntry NewEntry;
		NewEntry.InstanceId = FGuid::NewGuid();
		NewEntry.ItemData = ItemData;
		NewEntry.Quantity = Quantity;
		Items.Add(NewEntry);

		return true;
	}

	for (int32 Index = 0; Index < Quantity; ++Index)
	{
		FMuksiInventoryEntry NewEntry;
		NewEntry.InstanceId = FGuid::NewGuid();
		NewEntry.ItemData = ItemData;
		NewEntry.Quantity = 1;
		Items.Add(NewEntry);
	}

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

		if (Entry.Quantity < Quantity)
		{
			return false;
		}

		Entry.Quantity -= Quantity;

		if (Entry.Quantity <= 0)
		{
			Items.RemoveAt(Index);
		}

		return true;
	}

	return false;
}

bool UInventoryComponent::HasItem(FName ItemID, int32 RequiredQuantity) const
{
	if (ItemID.IsNone() || RequiredQuantity <= 0)
	{
		return false;
	}

	int32 FoundQuantity = 0;

	for (const FMuksiInventoryEntry& Entry : Items)
	{
		if (!Entry.ItemData)
		{
			continue;
		}

		if (Entry.ItemData->ItemID == ItemID)
		{
			FoundQuantity += Entry.Quantity;

			if (FoundQuantity >= RequiredQuantity)
			{
				return true;
			}
		}
	}

	return false;
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