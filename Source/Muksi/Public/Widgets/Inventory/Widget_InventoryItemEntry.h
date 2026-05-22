#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_InventoryItemEntry.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemEntrySelected, FGuid, InstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryItemEntryEquipRequested, FGuid, InstanceId);

UCLASS()
class MUKSI_API UWidget_InventoryItemEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEntry(const FMuksiInventoryEntry& InEntry, bool bInEquipped);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemEntrySelected OnSelected;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemEntryEquipRequested OnEquipRequested;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SelectButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> EquipButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EquippedText;

private:
	UFUNCTION()
	void HandleSelectClicked();

	UFUNCTION()
	void HandleEquipClicked();

	void Refresh();

	FMuksiInventoryEntry Entry;
	bool bEquipped = false;
};