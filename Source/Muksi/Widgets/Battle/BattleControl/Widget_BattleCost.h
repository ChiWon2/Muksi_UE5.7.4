// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleCost.generated.h"

class UBattleSkillComponent;
class UTextBlock;
class UWidget_BattleCostSlot;

class UHorizontalBox;


UENUM(BlueprintType)
enum class EBattleCostDisplayType : uint8
{
	Current,
	Displayed
};
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleCost : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(UBattleSkillComponent* InSkillComponent);
	
	void SetPreviewCost(int32 InCost);
	void ClearPreviewCost();

protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_CostSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Cost;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Cost")
	TSubclassOf<UWidget_BattleCostSlot> CostSlotClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EBattleCostDisplayType CostDisplayType = EBattleCostDisplayType::Current;

private:
	void HandleCostChanged();
	
	void InitializeCostSlots(int32 MaxCost);
	void RefreshCost();
	void UpdateCostSlots(int32 CurrentCost);
	void UnbindSkillComponent();

	UPROPERTY()
	TObjectPtr<UBattleSkillComponent> BattleSkillComponent;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCostSlot>> CostSlots;
	
	int32 PreviewCost = 0;
	
};
