// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Timer/Widget_BattleTimer.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UWidget_BattleTimer::NativeConstruct()
{
	Super::NativeConstruct();

	
	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(FText::AsNumber(0));
	}
	
	UpdateTimerDisplay(0.0f, 0.0f);
}

void UWidget_BattleTimer::ShowTimer(float TotalDuration)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	
	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(FText::AsNumber(FMath::CeilToInt(FMath::Max(TotalDuration, 0.0f))));
	}
	
	
	// 타이머 시작 시 Min = 0
	if (IsValid(TimerMaterial))
	{
		TimerMaterial->SetScalarParameterValue(TEXT("Min"),-0.38f);
	}
	
	const float Duration = FMath::Max(TotalDuration, 0.0f);
	const float InitialRatio = Duration > 0.0f ? 1.0f : 0.0f;

	UpdateTimerDisplay(Duration, InitialRatio);
}

void UWidget_BattleTimer::UpdateTimerDisplay(float RemainingTime, float RemainingRatio)
{
	
	const float Ratio = FMath::Clamp(RemainingRatio, 0.0f, 1.0f);

	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(
			FText::AsNumber(
				FMath::CeilToInt(FMath::Max(RemainingTime, 0.0f))
			)
		);
	}

	if (IsValid(ProgressBar_Timer))
	{
		ProgressBar_Timer->SetPercent(Ratio);

		// 남은 비율 1: 파랑, 0: 빨강
		const FLinearColor CurrentColor = FMath::Lerp(
			ProgressEndColor,
			ProgressStartColor,
			Ratio
		);

		ProgressBar_Timer->SetFillColorAndOpacity(CurrentColor);
	}
	
}

void UWidget_BattleTimer::StartWarning()
{

}

void UWidget_BattleTimer::ExpireTimer()
{
	UpdateTimerDisplay(
		0.0f,
		0.0f
	);

}






