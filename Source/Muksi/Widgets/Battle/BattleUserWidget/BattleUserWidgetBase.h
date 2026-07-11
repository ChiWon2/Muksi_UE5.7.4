// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleUserWidgetBase.generated.h"

/**
 * 
 */
class UWidget_BattleMainScreen;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUIAnimationEvent);

UCLASS()
class MUKSI_API UBattleUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetWidget_BattleMainScreen(UWidget_BattleMainScreen* WidgetBattleMainScreen){BattleMainScreen = WidgetBattleMainScreen;};
	
	protected:
	TObjectPtr<UWidget_BattleMainScreen> BattleMainScreen = nullptr;
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnUIAnimationEvent OnAnimationEvent;
	
	UFUNCTION(BlueprintCallable, Category="AnimEvent")
	virtual void AnimEventCalled();
};
