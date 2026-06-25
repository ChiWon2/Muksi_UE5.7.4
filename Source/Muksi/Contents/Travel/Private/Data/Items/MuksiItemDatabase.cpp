#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDatabase.h"

#include "Muksi/Contents/Travel/Public/Data/Items/MuksiItemDataAsset.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

UMuksiItemDataAsset* UMuksiItemDatabase::FindItemData(FName ItemID) const
{
	if (ItemID.IsNone())
	{
		return nullptr;
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

	TSet<const UMuksiItemDataAsset*> SeenItemAssets;


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

		if (SeenItemAssets.Contains(ItemData))
		{
			Context.AddWarning(FText::FromString(
				FString::Printf(
					TEXT("ItemData '%s' is registered multiple times. Current key: '%s'."),
					*GetNameSafe(ItemData),
					*MapKey.ToString())));
		}

		SeenItemAssets.Add(ItemData);
	}

	return Result;
}
#endif