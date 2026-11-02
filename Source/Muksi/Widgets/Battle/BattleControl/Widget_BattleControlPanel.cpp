// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleControlPanel.h"

#include "Widget_BattleSkillBar.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Timer/BattleTimerComponent.h"
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
	
	/*if (BattleTimerWidget)
	{
		BattleTimerWidget->HideTimer();
	}*/
	
	BaseRenderTranslation = GetRenderTransform().Translation;

	CurrentYOffset = 0.0f;
	TargetYOffset = 0.0f;
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

void UWidget_BattleControlPanel::BindPlayerSelectTimer(UBattleTimerComponent* InTimer)
{
	// 기존 타이머 연결 해제
	if (IsValid(BoundPlayerTimer))
	{
		BoundPlayerTimer->OnTimerStarted.RemoveDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerStarted);
		BoundPlayerTimer->OnTimerUpdated.RemoveDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerUpdated);
		BoundPlayerTimer->OnTimerWarning.RemoveDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerWarning);
		BoundPlayerTimer->OnTimerExpired.RemoveDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerExpired);
	}

	// 새로운 타이머 저장
	BoundPlayerTimer = InTimer;

	if (!IsValid(BoundPlayerTimer))
	{
		return;
	}

	BoundPlayerTimer->OnTimerStarted.AddUniqueDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerStarted);
	BoundPlayerTimer->OnTimerUpdated.AddUniqueDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerUpdated);
	BoundPlayerTimer->OnTimerWarning.AddUniqueDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerWarning);
	BoundPlayerTimer->OnTimerExpired.AddUniqueDynamic(this, &UWidget_BattleControlPanel::HandlePlayerTimerExpired);
}

void UWidget_BattleControlPanel::HideSelectTimer()
{
}

void UWidget_BattleControlPanel::HandlePlayerTimerStarted(float TotalDuration, float WarningTime)
{
	if (BattleTimerWidget)
		BattleTimerWidget->ShowTimer(TotalDuration);
}

void UWidget_BattleControlPanel::HandlePlayerTimerUpdated(float RemainingTime, float RemainingRatio)
{
	if (BattleTimerWidget)
		BattleTimerWidget->UpdateTimerDisplay(RemainingTime, RemainingRatio);
}

void UWidget_BattleControlPanel::HandlePlayerTimerWarning()
{
	if (BattleTimerWidget)
		BattleTimerWidget->StartWarning();
}

void UWidget_BattleControlPanel::HandlePlayerTimerExpired()
{
	if (BattleTimerWidget)
		BattleTimerWidget->ExpireTimer();
}

/*void UWidget_BattleControlPanel::StartExchangeTimer()
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

	/*if (BattleTimerWidget)
	{
		BattleTimerWidget->HideTimer();
	}#1#
}*/

void UWidget_BattleControlPanel::NativeTick(const FGeometry& Geometry, float InDeltaTime)
{
	Super::NativeTick(Geometry, InDeltaTime);
	
	// UI 위치 이동
	UpdateTargetingPosition(InDeltaTime);
	
	//Timer 설정
	if (!bExchangeTimerActive)
	{
		return;
	}

	//UpdateExchangeTimer(InDeltaTime);
}

/*void UWidget_BattleControlPanel::UpdateExchangeTimer(float DeltaTime)
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
}*/

void UWidget_BattleControlPanel::SetTargetingMode(bool bTargeting)
{
	TargetYOffset = bTargeting
		? TargetingYOffset
		: 0.0f;
}

void UWidget_BattleControlPanel::UpdateTargetingPosition(float DeltaTime)
{
	CurrentYOffset = FMath::FInterpTo(
		CurrentYOffset,
		TargetYOffset,
		DeltaTime,
		TargetingMoveSpeed
	);

	SetRenderTranslation(
		BaseRenderTranslation + FVector2D(0.0f, CurrentYOffset)
	);
}
