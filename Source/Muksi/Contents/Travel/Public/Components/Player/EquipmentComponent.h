#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "EquipmentComponent.generated.h"

class UInventoryComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItemByInstanceId(FGuid InstanceId);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(EMuksiEquipmentSlot Slot);

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsEquipped(FGuid InstanceId) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool IsSlotOccupied(EMuksiEquipmentSlot Slot) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool GetEquippedItemId(EMuksiEquipmentSlot Slot, FGuid& OutInstanceId) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool GetEquippedItem(EMuksiEquipmentSlot Slot, FMuksiInventoryEntry& OutEntry) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	bool GetTotalEquipmentStatModifier(FMuksiItemStatModifier& OutModifier) const;

	UFUNCTION(BlueprintPure, Category = "Equipment")
	TMap<EMuksiEquipmentSlot, FGuid> GetEquippedItemIds() const { return EquippedItemIds; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EMuksiEquipmentSlot, FGuid> EquippedItemIds;

private:
	const UInventoryComponent* GetInventoryComponent() const;
};
