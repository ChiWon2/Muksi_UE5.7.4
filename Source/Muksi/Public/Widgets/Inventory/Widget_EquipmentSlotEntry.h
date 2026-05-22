#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_EquipmentSlotEntry.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentSlotUnequipRequested, EMuksiEquipmentSlot, Slot);

UCLASS()
class MUKSI_API UWidget_EquipmentSlotEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeSlot(EMuksiEquipmentSlot InSlot, const FMuksiInventoryEntry& InEquippedEntry, bool bInHasEquippedEntry);

	UPROPERTY(BlueprintAssignable, Category = "Equipment")
	FOnEquipmentSlotUnequipRequested OnUnequipRequested;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SlotNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EquippedItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> UnequipButton;

private:
	UFUNCTION()
	void HandleUnequipClicked();

	void Refresh();

	EMuksiEquipmentSlot Slot = EMuksiEquipmentSlot::None;
	FMuksiInventoryEntry EquippedEntry;
	bool bHasEquippedEntry = false;
};