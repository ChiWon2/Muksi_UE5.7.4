// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestBattleCard.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UTestBattleCard : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void OnFlipMiddle();
};
