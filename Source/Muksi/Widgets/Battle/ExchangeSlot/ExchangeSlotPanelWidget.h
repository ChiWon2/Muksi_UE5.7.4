// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ExchangeSlotTypes.h"
#include "Blueprint/UserWidget.h"
#include "ExchangeSlotPanelWidget.generated.h"

class UHandWidget;
class UWidget_CardEquipSlot;
class UWidget_BattleCardBase;
class UMuksiBattleCardDataAsset;
class ABattleCharacterBase;
struct FReleasedExchangeCard;



/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyCardRevealFinished, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardReturnRequested,UWidget_BattleCardBase*);
UCLASS()
class MUKSI_API UExchangeSlotPanelWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
public:
	UWidget_CardEquipSlot* FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const;
	UWidget_CardEquipSlot* GetPlayerSlotByExchangeNumber(int32 ExchangeNumber) const;
	
	UFUNCTION(BlueprintCallable)
	FCardEquipSlotData GetSlotDataByExchangeNumber_Player(int32 InIndex);
	UFUNCTION(BlueprintCallable)
	FCardEquipSlotData GetSlotDataByExchangeNumber_Enemy(int32 InIndex);

	UFUNCTION(BlueprintCallable)
	UMuksiBattleCardDataAsset* GetExchangeDataIndex_Player(int32 InIndex);
	UFUNCTION(BlueprintCallable)
	UMuksiBattleCardDataAsset* GetExchangeDataIndex_Enemy(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void ConfirmExchangeInput(int32 InIndex);
	UFUNCTION(BlueprintCallable)
	void EnableExchangeSlots(int32 InIndex);
	void EnableExchangeSlot(int32 InIndex, bool bActive);
	
	bool EnemySelectedBattleCardFlip(int32 InIndex, bool bFront);
	
	UFUNCTION()
	void PlaceEnemySelectCard(UMuksiBattleCardDataAsset* SelectCard, int32 ExchangeCount);
	
	TArray<FReleasedExchangeCard> ReleasePlayerCards();
	TArray<FReleasedExchangeCard> ReleaseEnemyCards();
	
	FOnEnemyCardRevealFinished OnEnemyCardRevealFinished;
	FOnCardReturnRequested OnCardReturnRequested;
	
	//void SetOwningHandWidget(UHandWidget* InHandWidget);
protected:
	//BindWidget--------------------------------------------
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> PlayerExchangeSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> PlayerExchangeSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> PlayerExchangeSlot_3;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyExchangeSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyExchangeSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyExchangeSlot_3;
	//-----------------------------------------------------

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> PlayerExchangeSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> EnemyExchangeSlots;

	void HandleEnemySelectedCardFlipFinished(UWidget_BattleCardBase* CardWidget);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	TSubclassOf<UWidget_BattleCardBase> BattleCardClass;
private:
	void InitializeExchangeSlots();
	void HandleCardUnequipRequested(UWidget_CardEquipSlot* EquipSlot);
	
	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> EnemySelectedBattleCards;

	int32 PendingEnemyCardRevealIndex = INDEX_NONE;
	
	/*UPROPERTY()
	TObjectPtr<UHandWidget> OwningHandWidget;*/
};
