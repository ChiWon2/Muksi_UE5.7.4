// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Components/InkLineWidget.h"

#include "CommonLazyImage.h"
#include "CommonTextBlock.h"
#include "Components/SizeBox.h"

#include "MuksiDebugHelper.h"

void UInkLineWidget::SetInkText(const FText& InText)
{
	if (LabelText)
	{
		LabelText->SetText(InText);
	}

	RefreshInkWidth();
}

void UInkLineWidget::RefreshInkWidth()
{
	if (!LabelText || !InkSizeBox)
	{
		return;
	}

	LabelText->ForceLayoutPrepass();

	const FVector2D TextSize = LabelText->GetDesiredSize();
	
	float TextX = TextSize.X;
	FString Msg = FString::Printf(TEXT("Text size : %f"), TextX);
	Debug::Print(Msg);
	
	const float TargetWidth = TextSize.X + InkExtraWidth;

	InkSizeBox->SetWidthOverride(TargetWidth);
	InkSizeBox->SetHeightOverride(InkHeight);
	
	InkSizeBox->InvalidateLayoutAndVolatility();
}

void UInkLineWidget::PlayInkLine()
{
	bPlayForward = true;
	bAnimateInk = true;
}

void UInkLineWidget::ReverseInkLine()
{
	bPlayForward = false;
	bAnimateInk = true;
}

void UInkLineWidget::ResetInkLine()
{
	bAnimateInk = false;
	CurrentProgress = 0.f;

	if (InkMID)
	{
		InkMID->SetScalarParameterValue(TEXT("Progress"), 0.f);
	}
}

void UInkLineWidget::SetInkProgress(float InProgress)
{
	CurrentProgress = FMath::Clamp(InProgress, 0.f, 1.f);

	if (InkMID)
	{
		InkMID->SetScalarParameterValue(TEXT("Progress"), CurrentProgress);
	}
}

void UInkLineWidget::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshInkWidth();

	if (InkSizeBox)
	{
		InkSizeBox->SetHeightOverride(InkHeight);
	}

	if (InkImage)
	{
		InkMID = InkImage->GetDynamicMaterial();
		if (InkMID)
		{
			InkMID->SetScalarParameterValue(TEXT("Progress"), 0.f);
		}
	}
	
	SetInkText(InkText);
	
	CurrentProgress = 0.f;
	bPlayForward = false;
	bAnimateInk = false;
}

void UInkLineWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bAnimateInk || !InkMID)
	{
		return;
	}

	const float Direction = bPlayForward ? 1.f : -1.f;
	CurrentProgress += Direction * InDeltaTime * InkPlaySpeed;
	CurrentProgress = FMath::Clamp(CurrentProgress, 0.f, 1.f);

	InkMID->SetScalarParameterValue(TEXT("Progress"), CurrentProgress);

	if (CurrentProgress <= 0.f || CurrentProgress >= 1.f)
	{
		bAnimateInk = false;
	}
}
