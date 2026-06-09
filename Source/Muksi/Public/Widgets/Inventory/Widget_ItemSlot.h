#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_ItemSlot.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UBorder;
class UWidget_ItemTooltip;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotClicked, FGuid, InstanceId);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemSlotRightClicked, FGuid, InstanceId);

UCLASS()
class MUKSI_API UWidget_ItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(const FMuksiInventoryEntry& InEntry, bool bInEquipped);
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemSlotClicked OnClicked;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnItemSlotRightClicked OnRightClicked;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UBorder> SelectedBorder;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SlotButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> QuantityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EquippedText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnhanceLevelText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UWidget_ItemTooltip> TooltipClass;

private:
	UFUNCTION()
	void HandleSlotClicked();

	void Refresh();
	void RefreshTooltip();

	FMuksiInventoryEntry Entry;
	bool bEquipped = false;
	bool bSelected = false;
};