#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDatabase.h"

#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

void UMuksiItemDatabase::BuildItemDataMap() const
{
	ItemDataMap.Reset();

	for (UMuksiItemDataAsset* ItemData : ItemAssets)
	{
		if (!ItemData)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ItemDatabase] Null item asset skipped."));
			continue;
		}

		if (ItemData->ItemID.IsNone())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("[ItemDatabase] Item asset has empty ItemID. Asset=%s"),
				*GetNameSafe(ItemData));
			continue;
		}

		if (ItemDataMap.Contains(ItemData->ItemID))
		{
			UE_LOG(LogTemp, Error,
				TEXT("[ItemDatabase] Duplicate ItemID skipped. ItemID=%s Asset=%s"),
				*ItemData->ItemID.ToString(),
				*GetNameSafe(ItemData));
			continue;
		}

		ItemDataMap.Add(ItemData->ItemID, ItemData);
	}

	bItemDataMapBuilt = true;
}

UMuksiItemDataAsset* UMuksiItemDatabase::FindItemData(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return nullptr;
	}

	if (!bItemDataMapBuilt)
	{
		BuildItemDataMap();
	}

	const TObjectPtr<UMuksiItemDataAsset>* FoundItemData = ItemDataMap.Find(ItemID);

	return FoundItemData ? FoundItemData->Get() : nullptr;
}

bool UMuksiItemDatabase::ContainsItem(FName ItemID) const
{
	return FindItemData(ItemID) != nullptr;
}

#if WITH_EDITOR
EDataValidationResult UMuksiItemDatabase::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = EDataValidationResult::Valid;

	TSet<FName> SeenItemIDs;

	for (const TPair<FName, TObjectPtr<UMuksiItemDataAsset>>& Pair : ItemDataMap)
	{
		const FName MapKey = Pair.Key;
		const UMuksiItemDataAsset* ItemData = Pair.Value.Get();

		if (MapKey.IsNone())
		{
			Context.AddError(FText::FromString(TEXT("ItemDatabase contains an empty ItemID key.")));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (!ItemData)
		{
			Context.AddError(FText::FromString(
				FString::Printf(TEXT("ItemDatabase entry '%s' has no ItemData."), *MapKey.ToString())));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (ItemData->ItemID.IsNone())
		{
			Context.AddError(FText::FromString(
				FString::Printf(TEXT("ItemData '%s' has empty ItemID."), *GetNameSafe(ItemData))));
			Result = EDataValidationResult::Invalid;
			continue;
		}

		if (MapKey != ItemData->ItemID)
		{
			Context.AddError(FText::FromString(
				FString::Printf(
					TEXT("ItemDatabase key '%s' does not match ItemData '%s' ItemID '%s'."),
					*MapKey.ToString(),
					*GetNameSafe(ItemData),
					*ItemData->ItemID.ToString())));
			Result = EDataValidationResult::Invalid;
		}

		if (SeenItemIDs.Contains(ItemData->ItemID))
		{
			Context.AddError(FText::FromString(
				FString::Printf(TEXT("Duplicate ItemID detected: '%s'."), *ItemData->ItemID.ToString())));
			Result = EDataValidationResult::Invalid;
		}

		SeenItemIDs.Add(ItemData->ItemID);
	}

	return Result;
}
#endif