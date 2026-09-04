// Fill out your copyright notice in the Description page of Project Settings.


#include "ExchangeControlWidget.h"

#include "CommonButtonBase.h"
#include "Muksi/Widgets/Battle/Timer/Widget_BattleTimer.h"

void UExchangeControlWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BattleTimerWidget->HideTimer();
}


void UExchangeControlWidget::NativeTick(const FGeometry& Geometry, float TimeDelta)
{
	Super::NativeTick(Geometry, TimeDelta);
	
	if (!bExchangeTimerActive)
	{
		return;
	}

	UpdateExchangeTimer(TimeDelta);
}



void UExchangeControlWidget::StartExchangeTimer()
{
	ExchangeRemainingTime = ExchangeTotalDuration;

	bExchangeTimerActive = true;
	bWarningStarted = false;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->ShowTimer(ExchangeTotalDuration);
	}
}

void UExchangeControlWidget::StopExchangeTimer()
{
	bExchangeTimerActive = false;
	bWarningStarted = false;
	ExchangeRemainingTime = 0.0f;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->HideTimer();
	}
}

void UExchangeControlWidget::UpdateExchangeTimer(float DeltaTime)
{
	ExchangeRemainingTime = FMath::Max(ExchangeRemainingTime - DeltaTime,0.0f);

	const float RemainingRatio = ExchangeTotalDuration > 0.0f ? ExchangeRemainingTime / ExchangeTotalDuration: 0.0f;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->UpdateTimerDisplay(ExchangeRemainingTime, RemainingRatio);
	}

	// 경고 시작
	if (!bWarningStarted && ExchangeRemainingTime <= ExchangeWarningTime)
	{
		bWarningStarted = true;

		if (BattleTimerWidget)
		{
			BattleTimerWidget->StartWarning();
		}
	}

	// 시간 종료
	if (ExchangeRemainingTime <= 0.0f)
	{
		HandleExchangeTimerExpired();
	}
}

void UExchangeControlWidget::HandleExchangeTimerExpired()
{
	if (!bExchangeTimerActive)
	{
		return;
	}

	bExchangeTimerActive = false;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->ExpireTimer();
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Exchange Timer Expired")
	);
	
	OnExchangeTimeExpired.Broadcast();
}
