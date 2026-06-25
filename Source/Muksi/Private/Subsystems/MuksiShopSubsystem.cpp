#include "Subsystems/MuksiShopSubsystem.h"

#include "Muksi/Contents/Travel/Public/Data/Shops/ShopDataAsset.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

UMuksiShopSubsystem* UMuksiShopSubsystem::Get(
    const UObject* WorldContextObject)
{
    if (!WorldContextObject || !GEngine)
    {
        return nullptr;
    }

    UWorld* World = GEngine->GetWorldFromContextObject(
        WorldContextObject,
        EGetWorldErrorMode::ReturnNull);

    if (!World)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = World->GetGameInstance();

    return GameInstance
        ? GameInstance->GetSubsystem<UMuksiShopSubsystem>()
        : nullptr;
}

void UMuksiShopSubsystem::InitializeShopIfNeeded(
    const UShopDataAsset* ShopData)
{
    if (!ShopData || ShopData->ShopId.IsNone())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("[ShopStock] Initialize failed: invalid ShopData or ShopId"));
        return;
    }

    if (RuntimeShopStocks.Contains(ShopData->ShopId))
    {
        UE_LOG(LogTemp, Log,
            TEXT("[ShopStock] Initialize skipped Shop=%s AlreadyInitialized=1"),
            *ShopData->ShopId.ToString());
        return;
    }

    FRuntimeShopStock& RuntimeStock =
        RuntimeShopStocks.Add(ShopData->ShopId);

    for (const FShopItemEntry& Entry : ShopData->SellItems)
    {
        if (Entry.ItemID.IsNone() || Entry.bUnlimitedStock)
        {
            continue;
        }

        if (Entry.ShopEntryId.IsNone())
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[ShopStock] Entry skipped: ShopEntryId is None ItemID=%s"),
                *Entry.ItemID.ToString());
            continue;
        }

        if (RuntimeStock.StockByEntryId.Contains(Entry.ShopEntryId))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("[ShopStock] Duplicate ShopEntryId Shop=%s Entry=%s"),
                *ShopData->ShopId.ToString(),
                *Entry.ShopEntryId.ToString());
            continue;
        }

        RuntimeStock.StockByEntryId.Add(
            Entry.ShopEntryId,
            FMath::Max(0, Entry.Stock));
    }

    UE_LOG(LogTemp, Log,
        TEXT("[ShopStock] Initialize Shop=%s EntryCount=%d"),
        *ShopData->ShopId.ToString(),
        RuntimeStock.StockByEntryId.Num());
}

int32 UMuksiShopSubsystem::GetRemainingStock(
    const UShopDataAsset* ShopData,
    FName ShopEntryId) const
{
    if (!ShopData || ShopData->ShopId.IsNone() ||
        ShopEntryId.IsNone())
    {
        return 0;
    }

    const FRuntimeShopStock* ShopStock =
        RuntimeShopStocks.Find(ShopData->ShopId);

    if (!ShopStock)
    {
        return 0;
    }

    const int32* Remaining =
        ShopStock->StockByEntryId.Find(ShopEntryId);

    return Remaining ? *Remaining : 0;
}

bool UMuksiShopSubsystem::HasStock(
    const UShopDataAsset* ShopData,
    FName ShopEntryId,
    int32 Quantity) const
{
    return Quantity > 0 &&
        GetRemainingStock(ShopData, ShopEntryId) >= Quantity;
}

bool UMuksiShopSubsystem::TryReserveStock(
    const UShopDataAsset* ShopData,
    FName ShopEntryId,
    int32 Quantity)
{
    if (!ShopData || ShopData->ShopId.IsNone() ||
        ShopEntryId.IsNone() || Quantity <= 0)
    {
        return false;
    }

    FRuntimeShopStock* ShopStock =
        RuntimeShopStocks.Find(ShopData->ShopId);

    if (!ShopStock)
    {
        return false;
    }

    int32* Remaining =
        ShopStock->StockByEntryId.Find(ShopEntryId);

    if (!Remaining || *Remaining < Quantity)
    {
        return false;
    }

    *Remaining -= Quantity;

    UE_LOG(LogTemp, Log,
        TEXT("[ShopStock] Reserved Shop=%s Entry=%s Quantity=%d Remaining=%d"),
        *ShopData->ShopId.ToString(),
        *ShopEntryId.ToString(),
        Quantity,
        *Remaining);

    return true;
}

bool UMuksiShopSubsystem::ReleaseStock(
    const UShopDataAsset* ShopData,
    FName ShopEntryId,
    int32 Quantity)
{
    if (!ShopData || ShopData->ShopId.IsNone() ||
        ShopEntryId.IsNone() || Quantity <= 0)
    {
        return false;
    }

    FRuntimeShopStock* ShopStock =
        RuntimeShopStocks.Find(ShopData->ShopId);

    if (!ShopStock)
    {
        return false;
    }

    int32* Remaining =
        ShopStock->StockByEntryId.Find(ShopEntryId);

    if (!Remaining)
    {
        return false;
    }

    *Remaining += Quantity;

    UE_LOG(LogTemp, Warning,
        TEXT("[ShopStock] Released Shop=%s Entry=%s Quantity=%d Remaining=%d"),
        *ShopData->ShopId.ToString(),
        *ShopEntryId.ToString(),
        Quantity,
        *Remaining);

    return true;
}