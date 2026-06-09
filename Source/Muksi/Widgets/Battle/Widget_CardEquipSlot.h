// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HandWidget.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CardEquipSlot.generated.h"

class UMuksiBattleCardDataAsset;
class UBorder;
class UWidget_BattleCardBase;
class UCharacterDataBase;
class UHandWidget;
class UOverlay;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CardEquipSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;
	
	bool IsPointInsideSlot(const FVector2D& ScreenPosition) const;

	// 카드 위젯 자체를 슬롯에 장착
	bool EquipCard(UWidget_BattleCardBase* InCard);

	UFUNCTION(BlueprintPure)
	FVector2D GetSlotCenterInHandCanvas(UHandWidget* InHandWidget) const;

	UFUNCTION(BlueprintPure)
	FVector2D GetSlotSize() const;

	UFUNCTION(BlueprintPure)
	bool IsCardOverlappingSlot(UWidget_BattleCardBase* InCard) const;

	UFUNCTION(BlueprintCallable)
	bool ClearEquipSlot();
	
	UFUNCTION(BlueprintCallable)
	bool CheckEmptySlot();


public:
	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void SetSlotInfo(int32 InSlotIndex, int32 InExchangeNumber);

	// DataAsset 기반 카드 데이터 장착
	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void EquipCardData(
		UMuksiBattleCardDataAsset* InCardData,
		UCharacterDataBase* InSourceCharacter,
		UCharacterDataBase* InTargetCharacter
	);

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void SetBattleContext(
		UCharacterDataBase* InSourceCharacter,
		UCharacterDataBase* InTargetCharacter
	);

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void ClearSlot();

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void ForceClearSlot();

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void ConfirmSlot();

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void SetSlotEnabled(bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void SetSlotHighlighted(bool bInHighlighted);

	UFUNCTION(BlueprintCallable, Category = "CardEquipSlot")
	void SetSlotConfirmed(bool bConfirmed);

	UFUNCTION(BlueprintPure, Category = "CardEquipSlot")
	bool IsSlotEnabled() const { return bSlotEnabled; }

	UFUNCTION(BlueprintPure, Category = "CardEquipSlot")
	bool IsConfirmed() const { return SlotData.bConfirmed; }

	UFUNCTION(BlueprintPure, Category = "CardEquipSlot")
	bool HasCard() const { return SlotData.IsValidCard(); }

	UFUNCTION(BlueprintPure, Category = "CardEquipSlot")
	FCardEquipSlotData GetSlotData() const { return SlotData; }

protected:
	void RefreshSlotVisual();
	
	bool UnequipCard(UHandWidget* HandWidget);

protected:
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> CardHostOverlay;
	//***** BindWidget *****

	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> EquippedCard = nullptr;
	
	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	FCardEquipSlotData SlotData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	bool bSlotEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	bool bHighlighted = false;
	
public:
	UPROPERTY()
	TObjectPtr<UHandWidget> OwningHandWidget = nullptr;
};
