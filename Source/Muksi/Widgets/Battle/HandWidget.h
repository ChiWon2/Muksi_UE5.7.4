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
class UButton;
class UVerticalBox;
class ABattleCharacterBase;

class UMuksiCharacterDataAsset;
class UMuksiBattleCardDataAsset;
class UWidget_BattleMainScreen;
class UCharacterStatusWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurnRequested);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyCardRevealFinished, int32);

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
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;


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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
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

	UFUNCTION(BlueprintCallable)
	void InvisibleHandCards();

	UFUNCTION(BlueprintCallable)
	void VisibleHandCards();

	UFUNCTION(BlueprintCallable)
	void HitActiveHandCards(bool bHitActive);


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
	UWidget_CardEquipSlot* FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const;



	bool EnemySelectedBattleCardFlip(int32 InIndex, bool bFront);

	FOnEnemyCardRevealFinished OnEnemyCardRevealFinished;

	UPROPERTY()
	TObjectPtr<UWidget_BattleMainScreen> BattleMainScreen = nullptr;

	UFUNCTION()
	void PlaceEnemySelectCard(UMuksiBattleCardDataAsset* SelectCard, int32 ExchangeCount);
	UFUNCTION()
	void ClearEnemySelectCard();
	UFUNCTION()
	void ClearPlayerSelectCard();

protected:
	void HandleEnemySelectedCardFlipFinished(UWidget_BattleCardBase* CardWidget);

	//****** Bind Widget ******
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> HandCanvas;

	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EquipSlotTest;*/


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Button_Select;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInkLineWidget> InkLineWidget;


	//CardEquipSlot Box
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> CardEquipSlot_3;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> ExchangeSlots;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyCardEquipSlot_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyCardEquipSlot_2;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EnemyCardEquipSlot_3;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_CardEquipSlot>> EnemyExchangeSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> EnemySelectedBattleCards;

	int32 PendingEnemyCardRevealIndex = INDEX_NONE;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterStatusWidget> CharacterStatusWidget_Player;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterStatusWidget> CharacterStatusWidget_Enemy;

	//****** Bind Widget ******



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	TSubclassOf<UWidget_BattleCardBase> BattleCardClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float DefaultCardSpacing = 40.f;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> BattleCards;



	UPROPERTY()
	TArray<FWidgetCard> CardsStructArray;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> HoveredCard = nullptr;


	//***** Turn Changed UI Function *****
public:


	UPROPERTY(BlueprintAssignable, Category = "Hand|Event")
	FOnEndTurnRequested OnEndTurnRequested;


	void ShowTurnEndButton(bool bShow);

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
	void StartExchangeInput(int32 ExchangeNumber);

	UFUNCTION(BlueprintCallable)
	void InitializeExchangeSlots();


	UFUNCTION(BlueprintCallable)
	void EnableExchangeSlots(int32 InIndex);

	void EnableExchangeSlot(int32 InIndex, bool bActive);
	
	void ClearTimerHandler();

protected:
	void RequestOrganizeCards(float OffsetX = -1.0f);
	bool TryOrganizeCards(float OffsetX);

	bool bOrganizeCardsPending = false;
	bool bPlaceCardsAtDrawSpawnPending = false;
	float PendingCardSpacing = 0.0f;
	int32 OrganizeRetryCount = 0;

	// Wait for a fresh Slate layout instead of accepting stale-but-valid CachedGeometry.
	uint64 OrganizeNotBeforeFrame = 0;

	FTimerHandle OrganizeCardsTimerHandle;

	void ScheduleOrganizeCards();
	void HandleOrganizeCardsTimer();

	void BindSelectButton();
	void UnbindSelectButton();

	UFUNCTION()
	void HandleEndTurnButtonClicked();

	UWidget_CardEquipSlot* GetSlotByExchangeNumber(int32 ExchangeNumber) const;
	//***** Turn Changed UI Function *****

	//Ink Line 관련 Pipeline 관련 UI 호출
public:
	void DisplayInkLine(FString InText, float Time);
	void DisplayInkLinebActive();

	void DisplayInkLineEnabled(FString InText, float Time);
	void DisplayInkLineDisabled();


public:
	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void BuildHandFromCharacter(TArray<UMuksiBattleCardDataAsset*> BattleCardAssets);
	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void DrawCards(ABattleCharacterBase* BattleCharacter);
	bool HasHandCards() const { return !BattleCards.IsEmpty(); }

	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	UWidget_BattleCardBase* AddCardToHand(UMuksiBattleCardDataAsset* CardData);

	void PlaceCardInHand(UWidget_BattleCardBase* CardWidget);


protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CardDrawSpawnPoint;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CardDownPoint;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CardUpPoint;

	UPROPERTY()
	TObjectPtr<UWidget> HandCardPoint = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CardRemovePoint_Player;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> CardRemovePoint_Enemy;
	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> RemoveCardArray;

	UWidget_BattleCardBase* CreateCardAtDrawSpawnPoint(
	UMuksiBattleCardDataAsset* CardData);


public:
	FVector2D GetCardDrawStartLocalPosition() const;

public:

	void RemoveSelectedCardsData();

public:
	void SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);

};
