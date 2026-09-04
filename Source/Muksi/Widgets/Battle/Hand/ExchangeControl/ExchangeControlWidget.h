// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExchangeControlWidget.generated.h"


class UWidget_BattleTimer;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnExchangeTimeExpired);


UCLASS()
class MUKSI_API UExchangeControlWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& Geometry, float TimeDelta);
	
	
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
