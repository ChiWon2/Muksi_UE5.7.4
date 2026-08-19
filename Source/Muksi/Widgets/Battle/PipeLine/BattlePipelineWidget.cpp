// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/PipeLine/BattlePipelineWidget.h"

#include "Muksi/Widgets/Components/InkLineWidget.h"

void UBattlePipelineWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InkLineTimerHandle);
	}
	Super::NativeDestruct();
}

void UBattlePipelineWidget::DisplayInkLine(const FString& InText, float Time)
{
	if (!InkLineWidget)
	{
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InkLineTimerHandle);

		World->GetTimerManager().SetTimer(
			InkLineTimerHandle,
			this,
			&UBattlePipelineWidget::HandleInkLineFinished,
			Time,
			false
		);
	}

	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText(FText::FromString(InText));
	InkLineWidget->PlayInkLine();
}

void UBattlePipelineWidget::DisplayInkLineEnabled(const FString& InText, float Time)
{
	if (!InkLineWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("InkLineWidget is nullptr (HandWidget.cpp)"));
		return;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InkLineTimerHandle);

		World->GetTimerManager().SetTimer(InkLineTimerHandle, this, &UBattlePipelineWidget::DisplayInkLineDisabled, Time, false);
	}
	UE_LOG(LogTemp, Error, TEXT("DisplayInkLine Enabled %s"), *InText);
	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText(FText::FromString(InText));
	InkLineWidget->PlayInkLine();
}

void UBattlePipelineWidget::DisplayInkLineDisabled()
{
	if (!InkLineWidget)
	{
		return;
	}
	InkLineWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UBattlePipelineWidget::HandleInkLineFinished()
{
	if (InkLineWidget)
	{
		InkLineWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	OnPresentationFinished.Broadcast();
}
