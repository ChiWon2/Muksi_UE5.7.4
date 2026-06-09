#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_RepairItemEntry.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRepairItemEntryRightClicked, FGuid, InstanceId);

UCLASS()
class MUKSI_API UWidget_RepairItemEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeEntry(const FMuksiInventoryEntry& InEntry, int32 InRepairCost);
	void SetSelected(bool bInSelected);

	UPROPERTY(BlueprintAssignable, Category = "Forge|Repair")
	FOnRepairItemEntryRightClicked OnRightClicked;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnhanceLevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DurabilityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RepairCostText;

private:
	void Refresh();

	FMuksiInventoryEntry Entry;
	int32 RepairCost = 0;
	bool bSelected = false;
};