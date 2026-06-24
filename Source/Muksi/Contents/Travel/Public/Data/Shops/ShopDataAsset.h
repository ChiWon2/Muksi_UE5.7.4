#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ShopDataAsset.generated.h"

class UMuksiItemDataAsset;

USTRUCT(BlueprintType)
struct FShopItemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ShopEntryId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UMuksiItemDataAsset> ItemData = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 Price = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 Stock = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bUnlimitedStock = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"))
	int32 RequiredLevel = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText RequiredConditionText;
};

UCLASS(BlueprintType)
class MUKSI_API UShopDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ShopId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ShopName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FShopItemEntry> SellItems;
};