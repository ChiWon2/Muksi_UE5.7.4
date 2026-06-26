// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HandWidget.generated.h"

class UInkLineWidget;
class UHorizontalBox;
class UWidget_BattleCardBase;
class UWidget_CardEquipSlot;
class UCanvasPanel;
class UCanvasPanelSlot;
class UCommonButtonBase;
class UCharacterDataBase;
class UButton;
class UVerticalBox;

class UMuksiCharacterDataAsset;
class UMuksiBattleCardDataAsset;
class UWidget_BattleMainScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurnRequested);

USTRUCT(BlueprintType)
struct FWidgetCard
{
	GENERATED_BODY()
	
public:
	FWidgetCard();
	UPROPERTY()
	TObjectPtr<UWidget_BattleCardBase> Cards = nullptr;
	UPROPERTY()
	UCanvasPanelSlot* CanvasSlot = nullptr;
	UPROPERTY()
	int32 ZIndex = 0;
	
};

USTRUCT(BlueprintType)
struct FCardEquipSlotData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeNumber = 0;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterDataBase> SourceCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UCharacterDataBase> TargetCharacter = nullptr;

	UPROPERTY(BlueprintReadOnly)
	bool bConfirmed = false;

	bool IsValidCard() const
	{
		return CardData != nullptr && SourceCharacter != nullptr;
	}
};

/**
 * 
 */
UCLASS()
class MUKSI_API UHandWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	//~ End UUserWidget Interface
	
	UFUNCTION(BlueprintCallable)
	void SpawnDefaultHandCards();

	UFUNCTION(BlueprintCallable)
	void CreateTestHandCards(int32 InCount);

	UFUNCTION(BlueprintCallable)
	void OrganizeCards(float OffsetX);
	
	UFUNCTION(BlueprintCallable)
	void CreateCardMore();

	UFUNCTION(BlueprintCallable)
	void ClearHandCards();
	
	
	void SetHoveredCard(UWidget_BattleCardBase* InHoveredCard);
	void ClearHoveredCard(UWidget_BattleCardBase* InCard);
	
	float GetDefaultCardSpacing() const { return DefaultCardSpacing; }
	
	const FGeometry& GetHandCanvasGeometry() const;
	UCanvasPanel* GetHandCanvas() const { return HandCanvas; }
	
	UWidget_CardEquipSlot* GetEquipSlot() const;
	
	void RemoveBattleCards(UWidget_BattleCardBase* InCard);
	
	UFUNCTION()
	void OnClickedTurnEndButton();
	
	UFUNCTION(BlueprintCallable)
	void ActiveInkLine(FText& DisplayText,float DisplayTime = 3.f);
	
	UFUNCTION(BlueprintCallable)
	void DeActiveInkLine();
	
	UFUNCTION(BlueprintCallable)
	UWidget_CardEquipSlot* FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const;

	
	UPROPERTY()
	TObjectPtr<UWidget_BattleMainScreen> BattleMainScreen = nullptr;
	
	UFUNCTION()
	void PlaceEnemySelectCard(UMuksiBattleCardDataAsset* SelectCard);
	UFUNCTION()
	void ClearEnemySelectCard();
	
protected:
	//****** Bind Widget ******
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> HandCanvas;
	
	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EquipSlotTest;*/
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TurnEnd;
	
	UPROPERTY(meta = (BindWidget))
	UInkLineWidget* InkLine;
	
	//CardEquipSlot Box
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_3;
	
	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> ExchangeSlots;
	//****** Bind Widget ******
	
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	TSubclassOf<UWidget_BattleCardBase> BattleCardClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float LiteralFloatX = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float LiteralFloatY = 0.f;
	
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float HoverOffsetY = -20.f;*/

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float DefaultCardSpacing = 40.f;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> BattleCards;
	


	UPROPERTY()
	TArray<FWidgetCard> CardsStructArray;
	
	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> HoveredCard = nullptr;
	
	
	FTimerHandle TimerHandle;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Card Slot")
	TSubclassOf<UWidget_CardEquipSlot> CardEquipSlotClass;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> PlayerEquipSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> EnemyEquipSlots;
	
	//Test BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EnemySelectCardVerticalBox;
	
	
	//***** Turn Changed UI Function *****
public:
	
		
	UPROPERTY(BlueprintAssignable, Category = "Hand|Event")
	FOnEndTurnRequested OnEndTurnRequested;
	
	
	void ShowTurnEndButton(bool bShow);
	
	UFUNCTION(BlueprintCallable)
	FCardEquipSlotData GetSlotDataByExchangeNumber(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	UMuksiBattleCardDataAsset* GetExchangeDataIndex(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void ConfirmExchangeInput(int32 InIndex);
	
	UFUNCTION(BlueprintCallable)
	void StartExchangeInput(int32 ExchangeNumber);
	
	UFUNCTION(BlueprintCallable)
	void InitializeExchangeSlots();
	
	
	UFUNCTION(BlueprintCallable)
	void EnableExchangeSlots(int32 InIndex);	
	
	void EnableExchangeSlot(int32 InIndex, bool bActive);
	
protected:
	void BindEndTurnButton();
	void UnbindEndTurnButton();

	UFUNCTION()
	void HandleEndTurnButtonClicked();	
	
	UWidget_CardEquipSlot* GetSlotByExchangeNumber(int32 ExchangeNumber) const;
	//***** Turn Changed UI Function *****
	
	
	
	
	
public:
	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void BuildHandFromCharacter(TArray<UMuksiBattleCardDataAsset*> BattleCardAssets);

	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void AddCardToHand(UMuksiBattleCardDataAsset* CardData);
	
	void PlaceCardInHand(UWidget_BattleCardBase* CardWidget);
	
	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void BuildHandBattleStart();
	
};
