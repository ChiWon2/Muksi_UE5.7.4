#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Types/MuksiItemTypes.h"
#include "InventoryComponent.generated.h"

class UMuksiItemDataAsset;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UMuksiItemDataAsset* ItemData, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByInstanceId(FGuid InstanceId, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemID, int32 RequiredQuantity) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FMuksiInventoryEntry>& GetItems() const { return Items; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool FindItemByInstanceId(FGuid InstanceId, FMuksiInventoryEntry& OutEntry) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FMuksiInventoryEntry> Items;
};
