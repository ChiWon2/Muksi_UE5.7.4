// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleCostSlot.generated.h"

UENUM(BlueprintType)
enum class EBattleCostSlotState : uint8
{
	Empty,
	Filled,
	PreviewConsume
};

class UImage;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleCostSlot : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetFilled(bool bFilled);
	
	void SetState(EBattleCostSlotState eState);
	
	protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Frame;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Fill;
};
