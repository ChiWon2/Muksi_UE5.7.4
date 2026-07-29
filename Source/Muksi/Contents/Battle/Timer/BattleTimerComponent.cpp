// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Timer/BattleTimerComponent.h"

// Sets default values for this component's properties
UBattleTimerComponent::UBattleTimerComponent()
{
	//World의 TimerManager가 UpdateTimer를 일정 간격으로 호출하는 방식
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UBattleTimerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UBattleTimerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*
	 * 레벨 전환이나 BattleManager 제거 시
	 * 예약된 타이머 호출을 제거한다.
	 */
	ClearUpdateTimer();

	Super::EndPlay(EndPlayReason);
}

void UBattleTimerComponent::StartTimer(float InDuration, float InWarningTime)
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("BattleTimerComponent::StartTimer: World is invalid")
		);

		return;
	}

	/*
	 * 이미 실행 중이던 반복 타이머가 있다면 제거한다.
	 *
	 * 이때 OnTimerStopped는 발생시키지 않는다.
	 * 새로운 타이머로 교체하는 것이기 때문이다.
	 */
	ClearUpdateTimer();

	TotalDuration = FMath::Max(
		InDuration,
		0.0f
	);

	WarningTime = FMath::Clamp(
		InWarningTime,
		0.0f,
		TotalDuration
	);

	bWarningTriggered = false;

	if (TotalDuration <= 0.0f)
	{
		/*
		 * 0초 이하로 시작하면 즉시 만료 처리한다.
		 */
		bTimerRunning = true;
		ExpireTimer();
		return;
	}

	/*
	 * 현재 월드 시간으로부터 종료 시각을 계산한다.
	 */
	TimerEndWorldTime =
		World->GetTimeSeconds() + TotalDuration;

	bTimerRunning = true;

	/*
	 * 위젯이 최초 상태를 즉시 표시할 수 있도록
	 * TimerManager 등록 전에 시작 이벤트를 전달한다.
	 */
	OnTimerStarted.Broadcast(
		TotalDuration,
		WarningTime
	);

	OnTimerUpdated.Broadcast(
		TotalDuration,
		1.0f
	);

	/*
	 * UpdateInterval마다 UpdateTimer를 반복 호출한다.
	 */
	World->GetTimerManager().SetTimer(
		TimerUpdateHandle,
		this,
		&UBattleTimerComponent::UpdateTimer,
		UpdateInterval,
		true
	);

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Battle timer started. "
			"Duration: %.2f, Warning: %.2f"
		),
		TotalDuration,
		WarningTime
	);
}

void UBattleTimerComponent::StopTimer()
{
	if (!bTimerRunning)
	{
		return;
	}

	ClearUpdateTimer();

	bTimerRunning = false;
	bWarningTriggered = false;

	OnTimerStopped.Broadcast();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Battle timer stopped")
	);
}

float UBattleTimerComponent::GetRemainingTime() const
{
	if (!bTimerRunning)
	{
		return 0.0f;
	}

	const UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return 0.0f;
	}

	return FMath::Max(
		TimerEndWorldTime - World->GetTimeSeconds(),
		0.0f
	);
}

float UBattleTimerComponent::GetRemainingRatio() const
{
	if (!bTimerRunning ||
		TotalDuration <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(
		GetRemainingTime() / TotalDuration,
		0.0f,
		1.0f
	);
}

void UBattleTimerComponent::UpdateTimer()
{
	if (!bTimerRunning)
	{
		return;
	}

	const float RemainingTime = GetRemainingTime();
	const float RemainingRatio = GetRemainingRatio();

	/*
	 * UI에서 ProgressBar와 숫자를 갱신할 때 사용한다.
	 */
	OnTimerUpdated.Broadcast(
		RemainingTime,
		RemainingRatio
	);

	/*
	 * 경고 구간에 처음 진입한 순간 한 번만 발생시킨다.
	 */
	if (!bWarningTriggered &&
		RemainingTime <= WarningTime)
	{
		bWarningTriggered = true;

		OnTimerWarning.Broadcast();

		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"Battle timer warning started. "
				"Remaining: %.2f"
			),
			RemainingTime
		);
	}

	if (RemainingTime <= 0.0f)
	{
		ExpireTimer();
	}
}

void UBattleTimerComponent::ExpireTimer()
{
	/*
	 * 중복 만료 방지.
	 */
	if (!bTimerRunning)
	{
		return;
	}

	ClearUpdateTimer();

	bTimerRunning = false;
	bWarningTriggered = false;

	/*
	 * UI가 정확히 0을 표시하게 마지막 업데이트를 보낸다.
	 */
	OnTimerUpdated.Broadcast(
		0.0f,
		0.0f
	);

	OnTimerExpired.Broadcast();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Battle timer expired")
	);
}

void UBattleTimerComponent::ClearUpdateTimer()
{
	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		return;
	}

	World->GetTimerManager().ClearTimer(
		TimerUpdateHandle
	);
}



