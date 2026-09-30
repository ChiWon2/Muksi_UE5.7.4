// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Test/Test_Widget_BattleTimerProgeress.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"


void UTest_Widget_BattleTimerProgeress::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeTimer();
}

void UTest_Widget_BattleTimerProgeress::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!bTimerRunning || TotalTime <= 0.0f)
	{
		return;
	}

	RemainingTime -= InDeltaTime;

	if (RemainingTime <= 0.0f)
	{
		RemainingTime = 0.0f;
		bTimerRunning = false;
	}

	const float Percent = FMath::Clamp(
		RemainingTime / TotalTime,
		0.0f,
		1.0f
	);

	ProgressBar_Timer->SetPercent(Percent);

	UpdateTimerVisual(Percent);
}

void UTest_Widget_BattleTimerProgeress::InitializeTimer()
{
	RemainingTime = TotalTime;

	bTimerRunning = true;

	if (ProgressBar_Timer)
	{
		ProgressBar_Timer->SetPercent(1.0f);
	}
}

void UTest_Widget_BattleTimerProgeress::UpdateTimerVisual(float Percent)
{
	if (!ProgressBar_Timer || !Image_TimerHandle)
	{
		return;
	}

	const float BarWidth = ProgressBar_Timer->GetCachedGeometry().GetLocalSize().X;

	if (BarWidth <= 0.0f)
	{
		return;
	}

	const float X = (BarWidth * Percent- (HandleWidth * 0.5f));

	Image_TimerHandle->SetRenderTranslation(FVector2D(X, 0.0f));
}
