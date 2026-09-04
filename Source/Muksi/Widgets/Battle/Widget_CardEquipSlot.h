// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Hand/ExchangeSlot/ExchangeSlotTypes.h"
#include "Widget_CardEquipSlot.generated.h"

class UWidget_CardEquipSlot;
class UMuksiBattleCardDataAsset;
class UBorder;
class UWidget_BattleCardBase;
class UOverlay;
class UWidgetAnimation;

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardUnequipRequested, UWidget_CardEquipSlot*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTurnOrderTiltFinished, UWidget_CardEquipSlot*);
UCLASS()
class MUKSI_API UWidget_CardEquipSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnCardUnequipRequested OnCardUnequipRequested;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
	bool IsPointInsideSlot(const FVector2D& ScreenPosition) const;

	// 카드 위젯 자체를 슬롯에 장착
	bool EquipCard(UWidget_BattleCardBase* InCard);
	bool EquipCard_Enemy(UWidget_BattleCardBase* InCard);
	bool CanEquipCard(UWidget_BattleCardBase* InCard) const;
	
	UWidget_BattleCardBase* ReleaseCard();
	
	UWidget_BattleCardBase* GetEquipSlot(){return EquippedCard;}
	
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
	
	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CardEquipSlot")
	bool bPlayerSlot = true;

protected:
	void RefreshSlotVisual();


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
	
	//선 후공 슬롯 애니메이션----------------------------------------------------------------------------------------------
public:
	virtual void OnAnimationFinished_Implementation(const UWidgetAnimation* Animation) override;
	FOnTurnOrderTiltFinished OnTurnOrderTiltFinished;
	
	bool PlayTurnOrderTilt(bool bFirst);
	void PlayTurnOrderReset();
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_First;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_Second;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_Reset;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_First_Enemy;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_Second_Enemy;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_TurnorderTilit_Reset_Enemy;
	
	UPROPERTY(Transient)
	TObjectPtr<UWidgetAnimation> ActiveTurnOrderAnimation = nullptr;
	
	bool bWaitingForTurnOrderTiltFinish = false;
	void StopTurnOrderAnimations();
	//------------------------------------------------------------------------------------------------------------------
	
};
