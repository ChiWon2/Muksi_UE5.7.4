#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Muksi/Contents/Travel/Public/MuksiTypes/MuksiItemTypes.h"
#include "Widget_ForgeConfirmPopup.generated.h"

class UInventoryComponent;
class UPlayerCurrencyComponent;

class UButton;
class UTextBlock;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnForgeActionCompleted, FGuid, EMuksiForgeActionType);

UCLASS()
class MUKSI_API UWidget_ForgeConfirmPopup : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	void InitializeForgePopup(FGuid InInstanceId, EMuksiForgeActionType InActionType);

	FOnForgeActionCompleted OnForgeActionCompleted;

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ItemNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DurabilityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> EnhanceLevelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> GoldText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NotEnoughGoldText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CostText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SuccessRateText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FailureInfoText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ConfirmButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CancelButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ConfirmButtonText;

private:
	UFUNCTION()
	void HandleConfirmButtonClicked();

	UFUNCTION()
	void HandleCancelButtonClicked();

	void RefreshInfo();
	void RefreshRepairInfo(const FMuksiInventoryEntry& Entry, const UInventoryComponent* InventoryComponent);
	void RefreshEnhanceInfo(const FMuksiInventoryEntry& Entry, const UInventoryComponent* InventoryComponent);

	FText GetActionTitleText() const;
	FText GetConfirmButtonText() const;
	FText GetEnhanceResultText(EMuksiEnhanceResult Result) const;

	UInventoryComponent* GetInventoryComponent() const;
	UPlayerCurrencyComponent* GetCurrencyComponent() const;

	int32 GetCurrentActionCost(const UInventoryComponent* InventoryComponent) const;
	bool CanAffordCurrentAction(const UInventoryComponent* InventoryComponent) const;
	void RefreshGoldInfo(const UInventoryComponent* InventoryComponent);

	FGuid InstanceId;
	EMuksiForgeActionType ActionType = EMuksiForgeActionType::Repair;
};