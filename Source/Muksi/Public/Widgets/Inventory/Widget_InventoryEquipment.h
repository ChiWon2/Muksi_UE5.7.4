#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_InventoryEquipment.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;
class UInventoryComponent;
class UEquipmentComponent;
class UStatComponent;
class UWidget_InventoryItemEntry;
class UWidget_EquipmentSlotEntry;

UCLASS()
class MUKSI_API UWidget_InventoryEquipment : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> InventoryListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EquipmentSlotListBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SelectedItemText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StatPreviewText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RefreshButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UWidget_InventoryItemEntry> InventoryItemEntryClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UWidget_EquipmentSlotEntry> EquipmentSlotEntryClass;

private:
	UFUNCTION()
	void HandleRefreshClicked();

	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleInventoryItemSelected(FGuid InstanceId);

	UFUNCTION()
	void HandleInventoryItemEquipRequested(FGuid InstanceId);

	UFUNCTION()
	void HandleEquipmentSlotUnequipRequested(EMuksiEquipmentSlot EquipmentSlot);

	void RefreshAll();
	void RefreshInventoryList();
	void RefreshEquipmentSlots();
	void RefreshSelectedItemText();
	void RefreshStatPreviewText();

	void CreateEquipmentSlotEntry(EMuksiEquipmentSlot EquipmentSlot);

	UInventoryComponent* GetPlayerInventoryComponent() const;
	UEquipmentComponent* GetPlayerEquipmentComponent() const;
	UStatComponent* GetPlayerStatComponent() const;

	FGuid SelectedInstanceId;
};