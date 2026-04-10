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
class ABattleManager;
class UButton;

USTRUCT(BlueprintType)
struct FWidgetCard
{
	GENERATED_BODY()
	
public:
	FWidgetCard();
	UPROPERTY()
	TObjectPtr<UWidget_BattleCardBase> Cards = nullptr;
	UPROPERTY()
	UCanvasPanelSlot* CanvasSlot;
	UPROPERTY()
	int32 ZIndex = 0;
	
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
	
	UWidget_CardEquipSlot* GetEquipSlot() const { return EquipSlotTest; }
	
	void RemoveBattleCards(UWidget_BattleCardBase* InCard);
	
	UFUNCTION()
	void OnClickedTurnEndButton();
	
	UFUNCTION(BlueprintCallable)
	void ActiveInkLine(FText& DisplayText,float DisplayTime = 3.f);
	
	UFUNCTION(BlueprintCallable)
	void DeActiveInkLine();

protected:
	//****** Bind Widget ******
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> HandCanvas;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CardEquipSlot> EquipSlotTest;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_TurnEnd;
	
	UPROPERTY(meta = (BindWidget))
	UInkLineWidget* InkLine;
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
	
	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;
	
	FTimerHandle TimerHandle;
};
