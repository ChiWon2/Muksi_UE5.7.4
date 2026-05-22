#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_InventoryEquipmentTest.generated.h"

class UButton;
class UTextBlock;
class UVerticalBox;
class UMuksiItemDataAsset;
class UInventoryComponent;
class UEquipmentComponent;
class UStatComponent;

UCLASS()
class MUKSI_API UWidget_InventoryEquipmentTest : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ItemListBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SelectedItemText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EquippedItemText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> StatPreviewText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddTestItemButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EquipButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UnequipWeaponButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UnequipArmorButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> UnequipAccessoryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RefreshButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory Test")
	TArray<TObjectPtr<UMuksiItemDataAsset>> TestItemDataAssets;

	UFUNCTION()
	void HandleAddTestItemButtonClicked();

	UFUNCTION()
	void HandleEquipButtonClicked();

	UFUNCTION()
	void HandleUnequipWeaponButtonClicked();

	UFUNCTION()
	void HandleUnequipArmorButtonClicked();

	UFUNCTION()
	void HandleUnequipAccessoryButtonClicked();

	UFUNCTION()
	void HandleRefreshButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();

private:
	FGuid SelectedInstanceId;

	void RefreshAll();
	void RefreshItemList();
	void RefreshSelectedText();
	void RefreshEquippedText();
	void RefreshStatPreviewText();

	void SelectItem(FGuid InstanceId);
	void UnequipSlot(EMuksiEquipmentSlot EquipmentSlot);

	UInventoryComponent* GetPlayerInventoryComponent() const;
	UEquipmentComponent* GetPlayerEquipmentComponent() const;
	UStatComponent* GetPlayerStatComponent() const;

	UPROPERTY()
	TMap<TObjectPtr<UButton>, FGuid> ButtonToInstanceIdMap;

	UFUNCTION()
	void HandleDynamicItemButtonClicked();
};
