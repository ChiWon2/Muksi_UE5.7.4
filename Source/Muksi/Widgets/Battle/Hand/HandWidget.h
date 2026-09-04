// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HandWidget.generated.h"

struct FBattleCardInstance;
struct FCharacterPanicTimeoutResult;

class UExchangeSlotPanelWidget;
class UInkLineWidget;
class UHorizontalBox;
class UCanvasPanel;
class UCanvasPanelSlot;
class UCommonButtonBase;
class UButton;
class UVerticalBox;
class ABattleCharacterBase;

class UMuksiCharacterDataAsset;
class UMuksiBattleCardDataAsset;
class UBattleCardManager;
class UWidget_BattleCardBase;
class UWidget_CardEquipSlot;



DECLARE_MULTICAST_DELEGATE(FOnPlayerCardEquipped);
DECLARE_MULTICAST_DELEGATE(FOnPlayerCardReturned);

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
	void OrganizeCards(float OffsetX);

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
	

	void RemoveHandCardWidget(UWidget_BattleCardBase* InCard);


	
	

protected:
	

	//****** Bind Widget ******
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> HandCanvas;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UExchangeSlotPanelWidget> ExchangeSlotPanelWidget;
	
	//****** Bind Widget ******
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	TSubclassOf<UWidget_BattleCardBase> BattleCardClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle")
	float DefaultCardSpacing = 40.f;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleCardBase>> BattleCards;
	

	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> HoveredCard = nullptr;

protected:
	void RequestOrganizeCards(float OffsetX = -1.0f);
	bool TryOrganizeCards(float OffsetX);

	bool bOrganizeCardsPending = false;
	bool bPlaceCardsAtDrawSpawnPending = false;
	float PendingCardSpacing = 0.0f;
	int32 OrganizeRetryCount = 0;

	// Wait for a fresh Slate layout instead of accepting stale-but-valid CachedGeometry.
	uint64 OrganizeNotBeforeFrame = 0;
	
	void MoveCardToRemovePoint(UWidget_BattleCardBase* CardWidget, const FVector2D& StartAbsolutePosition, UWidget* RemovePoint);

	
	FTimerHandle OrganizeCardsTimerHandle;
	void ScheduleOrganizeCards();
	void HandleOrganizeCardsTimer();
	void ClearTimerHandler();
public:

	UFUNCTION(BlueprintCallable, Category = "Hand|Card")
	void DrawCards();
	bool HasHandCardWidgets()const;
	

	void PlaceCardInHand(UWidget_BattleCardBase* CardWidget);

	bool CommitHandCard(UWidget_BattleCardBase* CardWidget);
	bool ReturnCommittedHandCard(UWidget_BattleCardBase* CardWidget);
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
	const FBattleCardInstance& CardInstance);
public:
	FVector2D GetCardDrawStartLocalPosition() const;
public:
	void RemoveSelectedCardsData();
	
	void SetBattleCharacter(ABattleCharacterBase* InBattleCharacter);

	ABattleCharacterBase* GetBoundCharacter() const
	{
		return BoundCharacter;
	}
	
	//카드 변경 기능----------------------------------------------------------------------------------------------
public:
	void BindingBattleCardManager(UBattleCardManager* InBattleCardManager);
private:
	UFUNCTION()
	void HandleBattleHandCardChanged(FGuid InstanceId, UMuksiBattleCardDataAsset* NewCardData);
	
	UWidget_BattleCardBase* FindBattleCardWidgetByInstanceId(const FGuid& InstanceId) const;

	UPROPERTY(Transient)
	TObjectPtr<UBattleCardManager> BattleCardManager = nullptr;
	//----------------------------------------------------------------------------------------------------------
	//ExchangeSlot 관련 함수(이름이 바뀌거나 간소화 될 수 있음)-------------------------------------------------------
public:
	UFUNCTION()
	void ClearEnemySelectCard();
	UFUNCTION()
	void ClearPlayerSelectCard();
	UExchangeSlotPanelWidget* GetExchangeSlotPanelWidget()const{return ExchangeSlotPanelWidget;}
	UFUNCTION(BlueprintCallable)
	UWidget_CardEquipSlot* FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const;
	
	void EnableExchangeSlot(int32 Index, bool bActive);
	
	FOnPlayerCardEquipped OnPlayerCardEquipped;
	FOnPlayerCardReturned OnPlayerCardReturned;
	
	void NotifyPlayerCardEquipped();
private:
	void HandleCardReturnRequested(UWidget_BattleCardBase* CardWidget);

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> BoundCharacter;
	//-----------------------------------------------------------------------------------------------------------
	
	//타임아웃 관련 기능--------------------------------------------------------------------------------------------
public:
	bool ApplyPlayerPanicTimeoutResult(int32 ExchangeIndex, const FCharacterPanicTimeoutResult& Result);
};
