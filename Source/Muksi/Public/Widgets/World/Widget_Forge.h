// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widgets/World/Widget_ForgeConfirmPopup.h"
#include "Widget_Forge.generated.h"

class UInventoryComponent;
class UWidget_ItemSlot;
class UWidget_ForgeConfirmPopup;

class UButton;
class UUniformGridPanel;

UCLASS()
class MUKSI_API UWidget_Forge : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RepairButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EnhanceButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UUniformGridPanel> ForgeItemGridPanel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Forge")
	TSubclassOf<UWidget_ItemSlot> ForgeItemSlotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Forge")
	int32 ForgeItemColumnCount = 6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Forge")
	TSoftClassPtr<UWidget_ActivatableBase> ForgeConfirmPopupClass;


private:
	UFUNCTION()
	void HandleRepairButtonClicked();

	UFUNCTION()
	void HandleEnhanceButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();

	UFUNCTION()
	void HandleForgeItemClicked(FGuid InstanceId);

	void HandleForgeActionCompleted(FGuid InstanceId, EMuksiForgeActionType ActionType);

	void RefreshForgeItemList();
	void PushForgeConfirmPopup(FGuid InstanceId, EMuksiForgeActionType ActionType);

	UInventoryComponent* GetInventoryComponent() const;

	FGuid SelectedForgeItemInstanceId;
};
