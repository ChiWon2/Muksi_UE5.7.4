// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Timer/Widget_BattleTimer.h"

#include "Animation/WidgetAnimation.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWidget_BattleTimer::NativeConstruct()
{
	Super::NativeConstruct();
	//SetVisibility(ESlateVisibility::Collapsed);

	if (IsValid(TimerProgressBar))
	{
		TimerProgressBar->SetPercent(1.0f);
	}

	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(FText::AsNumber(0));
	}
}

void UWidget_BattleTimer::ShowTimer(float TotalDuration)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

	if (IsValid(TimerProgressBar))
	{
		TimerProgressBar->SetPercent(1.0f);
	}

	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(
			FText::AsNumber(
				FMath::CeilToInt(
					FMath::Max(TotalDuration, 0.0f)
				)
			)
		);
	}
}

void UWidget_BattleTimer::UpdateTimerDisplay(float RemainingTime, float RemainingRatio)
{
	if (IsValid(TimerProgressBar))
	{
		TimerProgressBar->SetPercent(
			FMath::Clamp(
				RemainingRatio,
				0.0f,
				1.0f
			)
		);
	}

	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(
			FText::AsNumber(
				FMath::CeilToInt(
					FMath::Max(RemainingTime, 0.0f)
				)
			)
		);
	}
}

void UWidget_BattleTimer::StartWarning()
{
	if (IsValid(WarningAnimation))
	{
		PlayAnimation(
			WarningAnimation,
			0.0f,
			0
		);
	}
}

void UWidget_BattleTimer::ExpireTimer()
{
	UpdateTimerDisplay(
		0.0f,
		0.0f
	);

	if (IsValid(ExpiredAnimation))
	{
		PlayAnimation(ExpiredAnimation);
	}
	else
	{
		HideTimer();
	}
}

void UWidget_BattleTimer::HideTimer()
{
	if (IsValid(WarningAnimation) && IsAnimationPlaying(WarningAnimation))
	{
		StopAnimation(WarningAnimation);
	}

	if (IsValid(ExpiredAnimation) && IsAnimationPlaying(ExpiredAnimation))
	{
		StopAnimation(ExpiredAnimation);
	}

	SetVisibility(ESlateVisibility::Collapsed);
}
