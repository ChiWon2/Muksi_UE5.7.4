#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MuksiShopSubsystem.generated.h"

class UShopDataAsset;

struct FRuntimeShopStock
{
    TMap<FName, int32> StockByEntryId;
};

UCLASS()
class MUKSI_API UMuksiShopSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category = "Shop",
        meta = (WorldContext = "WorldContextObject"))
    static UMuksiShopSubsystem* Get(const UObject* WorldContextObject);

    void InitializeShopIfNeeded(const UShopDataAsset* ShopData);

    int32 GetRemainingStock(
        const UShopDataAsset* ShopData,
        FName ShopEntryId) const;

    bool HasStock(
        const UShopDataAsset* ShopData,
        FName ShopEntryId,
        int32 Quantity = 1) const;

    bool TryReserveStock(
        const UShopDataAsset* ShopData,
        FName ShopEntryId,
        int32 Quantity = 1);

    bool ReleaseStock(
        const UShopDataAsset* ShopData,
        FName ShopEntryId,
        int32 Quantity = 1);

private:

    TMap<FName, FRuntimeShopStock> RuntimeShopStocks;
};