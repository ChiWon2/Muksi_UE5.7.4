#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "Widget_EquipmentSlot.generated.h"

class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentSlotClicked, EMuksiEquipmentSlot, EquipmentSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentSlotRightClicked, EMuksiEquipmentSlot, EquipmentSlot);

UCLASS()
class MUKSI_API UWidget_EquipmentSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(EMuksiEquipmentSlot InSlot, const FMuksiInventoryEntry& InEquippedEntry, bool bInHasEquippedItem);
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentSlotClicked OnClicked;

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentSlotRightClicked OnRightClicked;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SlotButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> ItemIconImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EmptyText;

private:
	UFUNCTION()
	void HandleSlotClicked();

	void Refresh();

	EMuksiEquipmentSlot Slot = EMuksiEquipmentSlot::None;
	FMuksiInventoryEntry EquippedEntry;
	bool bHasEquippedItem = false;
	bool bSelected = false;
};