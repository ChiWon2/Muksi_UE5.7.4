// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/PipeLine/BattlePipelineWidget.h"

#include "InkLineWidget.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"


void UBattlePipelineWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InkLineTimerHandle);
	}
	Super::NativeDestruct();
}

void UBattlePipelineWidget::DisplayPhase(FBattlePhasePresentationContext& PhaseContext)
{
	HandleBattlePipelineFinishCount = 0;
	//만약 Block 되어야 하는 UI가 있다면 그것 먼저 호출할것
	//지금 문제가 만약 Block이 안되어도 되는 UI만 2개 이상 있다면?
	switch (PhaseContext.Phase)
	{
	case EBattlePhase::BattleStart:
		{
			DisplayInkLine(BattleStartText, InkTime);
			break;
		}

	case EBattlePhase::RoundStart:
		{
			FString DisplayText =
				FString::Printf(TEXT("%d "), PhaseContext.RoundNum + 1)
				+ RoundStartText;
			DisplayInkLine(DisplayText, InkTime);
			break;
		}

	case EBattlePhase::ExchangeStart:
		{
			DisplayInkLine(ExchangeStartText, InkTime);
			break;
		}

	case EBattlePhase::CardSelect:
		{
			FString DisplayText =
				FString::Printf(TEXT("%d "), PhaseContext.ExchangeNum + 1)
				+ ExchangeCountText;

			DisplayInkLineEnabled(DisplayText, InkTime);
			break;
		}

	case EBattlePhase::ExchangeEnd:
		{
			DisplayInkLine(ExchangeEndText, InkTime);
			break;
		}

	case EBattlePhase::BattleActionSequenceStart:
		{
			DisplayInkLine(BattleActionSequenceStartText, InkTime);
			break;
		}

	case EBattlePhase::BattleActionSequenceEnd:
		{
			DisplayInkLine(BattleActionSequenceEndText, InkTime);
			break;
		}

	case EBattlePhase::RoundEnd:
		{
			DisplayInkLine(RoundEndText, InkTime);
			break;
		}

	case EBattlePhase::BattleEnd:
		{
			DisplayInkLine(BattleEndText, InkTime);
			break;
		}

	default:
		break;
	}
}

void UBattlePipelineWidget::HandleBattlePipelineFinished()
{
	//여기서 처리하던 UI 이벤트 다 끝났으면 Widget_BattleMainScreen에 호출
	if (HandleBattlePipelineFinishCount <= 0)
	{
		OnPresentationFinished.Broadcast();
	}
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
	
	HandleBattlePipelineFinishCount += 1;
	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText(FText::FromString(InText));
	InkLineWidget->PlayInkLine();
}

void UBattlePipelineWidget::DisplayInkLineEnabled(const FString& InText, float Time)
{
	//Phase가 대기할 필요 없는 그냥 InkLine
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
	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText(FText::FromString(InText));
	InkLineWidget->PlayInkLine();
	
	HandleBattlePipelineFinished();//Widget_BattleMainScreen가 이 UI가 끝날때까지 기다릴 필요는 없으니
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
	HandleBattlePipelineFinishCount -= 1;
	HandleBattlePipelineFinished();
}
