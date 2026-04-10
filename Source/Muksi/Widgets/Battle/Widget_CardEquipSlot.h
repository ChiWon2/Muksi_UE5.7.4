// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CardEquipSlot.generated.h"

class UBorder;
class UWidget_BattleCardBase;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CardEquipSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	bool IsPointInsideSlot(const FVector2D& ScreenPosition) const;
	void EquipCard(UWidget_BattleCardBase* InCard);
	
	
	UFUNCTION(BlueprintPure)
	FVector2D GetSlotCenterInHandCanvas() const;

	UFUNCTION(BlueprintPure)
	FVector2D GetSlotSize() const;

	UFUNCTION(BlueprintPure)
	bool IsCardOverlappingSlot(UWidget_BattleCardBase* InCard) const;
	
	UFUNCTION(BlueprintCallable)
	bool ClearEquipSlot();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> SlotBorder;

	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleCardBase> EquippedCard = nullptr;
};
