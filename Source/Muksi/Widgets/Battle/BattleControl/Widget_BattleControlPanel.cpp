// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleControlPanel.h"

#include "Widget_BattleSkillBar.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Timer/Widget_BattleTimer.h"

void UWidget_BattleControlPanel::SetBattleCharacter(ABattleCharacterBase* InCharacter)
{
	if (!BattleSkillBar)
	{
		return;
	}

	BattleSkillBar->SetBattleCharacter(InCharacter);
}

void UWidget_BattleControlPanel::RefreshSkillSlots()
{
	if (!BattleSkillBar)
	{
		return;
	}

	BattleSkillBar->RefreshSkillSlots();
}

void UWidget_BattleControlPanel::RefreshSkillBar()
{
	if (!BattleSkillBar)
	{
		return;
	}

	BattleSkillBar->RefreshSkillSlots();
}


void UWidget_BattleControlPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BattleSkillBar)
	{
		BattleSkillBar->OnBattleSkillSelected.AddUObject(this,&UWidget_BattleControlPanel::HandleBattleSkillSelected);
	}
	
	if (BattleTimerWidget)
	{
		BattleTimerWidget->HideTimer();
	}
}

void UWidget_BattleControlPanel::HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleControlPanel] Skill Selected Card=%s"),
		*GetNameSafe(CardData)
	);
	
	OnBattleSkillSelected.Broadcast(InstanceId, CardData);
}

void UWidget_BattleControlPanel::StartExchangeTimer()
{
	ExchangeRemainingTime = ExchangeTotalDuration;

	bExchangeTimerActive = true;
	bWarningStarted = false;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->ShowTimer(
			ExchangeTotalDuration
		);
	}
}

void UWidget_BattleControlPanel::StartExchangeTimer(int32 ExchangeIndex)
{
	if (ExchangeDurationCurve)
	{
		ExchangeTotalDuration =
			ExchangeDurationCurve->GetFloatValue(
				static_cast<float>(ExchangeIndex)
			);
	}

	ExchangeRemainingTime = ExchangeTotalDuration;

	bExchangeTimerActive = true;
	bWarningStarted = false;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->ShowTimer(
			ExchangeTotalDuration
		);
	}
}

void UWidget_BattleControlPanel::StopExchangeTimer()
{
	bExchangeTimerActive = false;
	bWarningStarted = false;

	ExchangeRemainingTime = 0.0f;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->HideTimer();
	}
}

void UWidget_BattleControlPanel::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);
	
	if (!bExchangeTimerActive)
	{
		return;
	}

	UpdateExchangeTimer(InDeltaTime);
}

void UWidget_BattleControlPanel::UpdateExchangeTimer(float DeltaTime)
{
	ExchangeRemainingTime = FMath::Max(
		ExchangeRemainingTime - DeltaTime,
		0.0f
	);

	const float RemainingRatio =
		ExchangeTotalDuration > 0.0f
			? ExchangeRemainingTime / ExchangeTotalDuration
			: 0.0f;

	if (BattleTimerWidget)
	{
		BattleTimerWidget->UpdateTimerDisplay(
			ExchangeRemainingTime,
			RemainingRatio
		);
	}

	// 경고 시작
	if (!bWarningStarted &&
		ExchangeRemainingTime <= ExchangeWarningTime)
	{
		bWarningStarted = true;

		if (BattleTimerWidget)
		{
			BattleTimerWidget->StartWarning();
		}
	}

	if (ExchangeRemainingTime <= 0.0f)
	{
		HandleExchangeTimerExpired();
	}
}

void UWidget_BattleControlPanel::HandleExchangeTimerExpired()
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
