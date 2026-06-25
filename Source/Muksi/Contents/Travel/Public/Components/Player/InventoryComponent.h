#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "InventoryComponent.generated.h"

class UMuksiItemDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnInventoryChanged, FName, ItemID, int32, OldQuantity, int32, NewQuantity);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();
public:
	UPROPERTY(BlueprintAssignable)
	FOnInventoryChanged OnInventoryChanged;
public:
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FName ItemID, UMuksiItemDataAsset* ItemData, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItemByInstanceId(FGuid InstanceId, int32 Quantity);

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemID, int32 RequiredQuantity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemCountByItemID(const FName& ItemID) const;

	UFUNCTION(BlueprintPure, Category = "Inventory")
	const TArray<FMuksiInventoryEntry>& GetItems() const { return Items; }

	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool FindItemByInstanceId(FGuid InstanceId, FMuksiInventoryEntry& OutEntry) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Repair")
	bool IsRepairableItem(FGuid InstanceId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Repair")
	int32 GetRepairCost(FGuid InstanceId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Repair")
	bool RepairItem(FGuid InstanceId, int32& OutRepairCost);

	UFUNCTION(BlueprintPure, Category = "Inventory|Repair")
	TArray<FMuksiInventoryEntry> GetRepairableItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Forge")
	TArray<FMuksiInventoryEntry> GetForgeableEquipmentItems() const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Enhance")
	bool IsEnhanceableItem(FGuid InstanceId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Enhance")
	int32 GetEnhanceCost(FGuid InstanceId) const;

	UFUNCTION(BlueprintPure, Category = "Inventory|Enhance")
	float GetEnhanceSuccessRate(FGuid InstanceId) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Enhance")
	bool EnhanceItem(FGuid InstanceId, int32& OutEnhanceCost, EMuksiEnhanceResult& OutResult);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FMuksiInventoryEntry> Items;

private:
	FMuksiInventoryEntry* FindMutableItemByInstanceId(FGuid InstanceId);
};
