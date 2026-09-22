// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Timer/Widget_BattleTimer.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UWidget_BattleTimer::NativeConstruct()
{
	Super::NativeConstruct();
	//SetVisibility(ESlateVisibility::Collapsed);
	

	if (IsValid(RemainingTimeText))
	{
		RemainingTimeText->SetText(FText::AsNumber(0));
	}
	
	// RopeBurnImage에 설정된 UI Material에서
	// Dynamic Material Instance 생성
	if (IsValid(BottleTimerImage))
	{
		TimerMaterial = BottleTimerImage->GetDynamicMaterial();

		if (IsValid(TimerMaterial))
		{
			TimerMaterial->SetScalarParameterValue(TEXT("Min"),0.0f);
		}
	}
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
		TimerMaterial->SetScalarParameterValue(TEXT("Min"),0.0f);
	}
}

void UWidget_BattleTimer::UpdateTimerDisplay(float RemainingTime, float RemainingRatio)
{
	
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
	
	UpdateTimerMaterial(RemainingRatio);
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

void UWidget_BattleTimer::HideTimer()
{
	
	//SetVisibility(ESlateVisibility::Collapsed);
}

void UWidget_BattleTimer::UpdateTimerMaterial(float RemainingRatio)
{
	/*
	 * RemainingRatio
	 *
	 * 시작 : 1.0
	 * 종료 : 0.0
	 *
	 * Material Min
	 *
	 * 시작 : 0.0
	 * 종료 : 0.85
	 */
	
	if (!IsValid(TimerMaterial))
	{
		return;
	}
	
	const float ElapsedRatio = 1.0f - RemainingRatio;

	const float MinValue = FMath::Lerp(0.0f,MaxMinValue,ElapsedRatio);

	TimerMaterial->SetScalarParameterValue(TEXT("Min"), MinValue);
	
	const FLinearColor CurrentColor = GetTimerColor(ElapsedRatio);
	TimerMaterial->SetVectorParameterValue(TEXT("Color"), CurrentColor);
}

FLinearColor UWidget_BattleTimer::GetTimerColor(float ElapsedRatio) const
{
	if (ElapsedRatio < 0.5f)
	{
		const float Alpha = ElapsedRatio * 2.0f;

		return FMath::Lerp(StartColor,MiddleColor,Alpha);
	}

	const float Alpha = (ElapsedRatio - 0.5f) * 2.0f;

	return FMath::Lerp(MiddleColor,EndColor,Alpha);
}
