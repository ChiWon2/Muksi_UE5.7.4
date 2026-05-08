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

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CardEquipSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	bool IsPointInsideSlot(const FVector2D& ScreenPosition) const;

	// 카드 위젯 자체를 슬롯에 장착
	void EquipCard(UWidget_BattleCardBase* InCard);

	UFUNCTION(BlueprintPure)
	FVector2D GetSlotCenterInHandCanvas() const;

	UFUNCTION(BlueprintPure)
	FVector2D GetSlotSize() const;

	UFUNCTION(BlueprintPure)
	bool IsCardOverlappingSlot(UWidget_BattleCardBase* InCard) const;

	UFUNCTION(BlueprintCallable)
	bool ClearEquipSlot();

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

protected:
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;
	//***** BindWidget *****

	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> EquippedCard = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	FCardEquipSlotData SlotData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	bool bSlotEnabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CardEquipSlot")
	bool bHighlighted = false;
};
