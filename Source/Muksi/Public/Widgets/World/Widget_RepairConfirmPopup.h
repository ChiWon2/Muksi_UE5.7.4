#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_RepairConfirmPopup.generated.h"

class UButton;
class UTextBlock;
class UInventoryComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRepairCompleted, FGuid);

UCLASS()
class MUKSI_API UWidget_RepairConfirmPopup : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	void InitializeRepairPopup(FGuid InInstanceId);

	FOnRepairCompleted OnRepairCompleted;

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DurabilityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RepairCostText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> RepairButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

private:
	UFUNCTION()
	void HandleRepairButtonClicked();

	UFUNCTION()
	void HandleCancelButtonClicked();

	void RefreshInfo();
	UInventoryComponent* GetInventoryComponent() const;

	FGuid InstanceId;
};