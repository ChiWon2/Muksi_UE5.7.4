// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExchangeControlWidget.generated.h"

class UCommonButtonBase;
class UWidget_BattleTimer;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnExchangeTimeExpired);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurnRequested);

UCLASS()
class MUKSI_API UExchangeControlWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float TimeDelta);

	UPROPERTY(BlueprintAssignable, Category = "Exchange|Event")
	FOnEndTurnRequested OnEndTurnRequested;
	
	//아래의 내용은 없어질 예정
	//CardSelect End Button------------------------------------------
public:
	void ShowSelectButton(bool bShow);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Button_Select;

private:
	void BindSelectButton();
	void UnbindSelectButton();

	void HandleSelectButtonClicked();
	//----------------------------------------------------------------
	
	//턴 시간 표시 위젯-------------------------------------------------
public:
	FOnExchangeTimeExpired OnExchangeTimeExpired;
	
	void StartExchangeTimer();
	void StopExchangeTimer();
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleTimer> BattleTimerWidget;
	

	void UpdateExchangeTimer(float DeltaTime);
	void HandleExchangeTimerExpired();

private:

	float ExchangeTotalDuration = 15.0f;

	float ExchangeRemainingTime = 0.0f;

	float ExchangeWarningTime = 5.0f;

	bool bExchangeTimerActive = false;

	bool bWarningStarted = false;
	//----------------------------------------------------------------
};
