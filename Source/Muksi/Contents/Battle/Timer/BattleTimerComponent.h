// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleTimerComponent.generated.h"

/**
 * 타이머가 시작됐을 때 호출된다.
 *
 * TotalDuration:
 * 전체 제한시간
 *
 * WarningTime:
 * 경고 연출을 시작할 남은 시간
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBattleTimerStarted,
	float,
	TotalDuration,
	float,
	WarningTime
);

/**
 * 타이머가 갱신될 때 호출된다.
 *
 * RemainingTime:
 * 현재 남은 시간
 *
 * RemainingRatio:
 * 전체 시간 대비 남은 시간 비율
 * 1.0 = 시작
 * 0.0 = 종료
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnBattleTimerUpdated,
	float,
	RemainingTime,
	float,
	RemainingRatio
);

/**
 * 경고 구간에 들어갔을 때 한 번 호출된다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleTimerWarning
);

/**
 * 시간이 0초가 되었을 때 호출된다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleTimerExpired
);

/**
 * 타이머가 수동으로 정지됐을 때 호출된다.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnBattleTimerStopped
);

UCLASS(
	ClassGroup = (Battle),
	meta = (BlueprintSpawnableComponent)
)
class MUKSI_API UBattleTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleTimerComponent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(
		const EEndPlayReason::Type EndPlayReason
	) override;

public:
	/**
	 * 타이머를 새로 시작한다.
	 *
	 * 이미 타이머가 실행 중이면 기존 타이머를 초기화하고
	 * 새로운 시간으로 다시 시작한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StartTimer(
		float InDuration,
		float InWarningTime = 10.0f
	);

	/**
	 * 실행 중인 타이머를 수동으로 정지한다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StopTimer();

	/**
	 * 현재 남은 시간을 반환한다.
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Timer")
	float GetRemainingTime() const;

	/**
	 * 전체 시간 대비 현재 남은 시간의 비율을 반환한다.
	 *
	 * 시작 직후: 1.0
	 * 절반 경과: 0.5
	 * 종료: 0.0
	 */
	UFUNCTION(BlueprintPure, Category = "Battle|Timer")
	float GetRemainingRatio() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Timer")
	float GetTotalDuration() const
	{
		return TotalDuration;
	}

	UFUNCTION(BlueprintPure, Category = "Battle|Timer")
	float GetWarningTime() const
	{
		return WarningTime;
	}

	UFUNCTION(BlueprintPure, Category = "Battle|Timer")
	bool IsTimerRunning() const
	{
		return bTimerRunning;
	}

public:
	UPROPERTY(BlueprintAssignable, Category = "Battle|Timer|Event")
	FOnBattleTimerStarted OnTimerStarted;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Timer|Event")
	FOnBattleTimerUpdated OnTimerUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Timer|Event")
	FOnBattleTimerWarning OnTimerWarning;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Timer|Event")
	FOnBattleTimerExpired OnTimerExpired;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Timer|Event")
	FOnBattleTimerStopped OnTimerStopped;

private:
	/**
	 * TimerManager가 일정 주기로 호출하는 함수.
	 */
	void UpdateTimer();

	/**
	 * 시간이 0초가 됐을 때 실행한다.
	 */
	void ExpireTimer();

	/**
	 * TimerManager에 등록된 반복 타이머만 제거한다.
	 */
	void ClearUpdateTimer();

private:
	/**
	 * 타이머 전체 제한시간.
	 */
	UPROPERTY(
		VisibleInstanceOnly,
		BlueprintReadOnly,
		Category = "Battle|Timer",
		meta = (AllowPrivateAccess = "true")
	)
	float TotalDuration = 0.0f;

	/**
	 * 경고 이벤트가 발생할 남은 시간.
	 *
	 * 예:
	 * 전체 30초, WarningTime 10초라면
	 * 남은 시간이 10초 이하가 되는 순간 경고 발생.
	 */
	UPROPERTY(
		VisibleInstanceOnly,
		BlueprintReadOnly,
		Category = "Battle|Timer",
		meta = (AllowPrivateAccess = "true")
	)
	float WarningTime = 0.0f;

	/**
	 * 타이머가 종료될 월드 시간.
	 *
	 * 매번 DeltaTime을 직접 빼지 않고:
	 *
	 * 종료 시각 - 현재 시각
	 *
	 * 으로 남은 시간을 계산한다.
	 */
	float TimerEndWorldTime = 0.0f;

	/**
	 * 현재 타이머가 실행 중인지 나타낸다.
	 */
	UPROPERTY(
		VisibleInstanceOnly,
		BlueprintReadOnly,
		Category = "Battle|Timer",
		meta = (AllowPrivateAccess = "true")
	)
	bool bTimerRunning = false;

	/**
	 * 현재 타이머에서 경고 이벤트가 이미 발생했는지 나타낸다.
	 *
	 * 이 값이 없으면 남은 시간이 10초 이하인 동안
	 * OnTimerWarning이 계속 호출된다.
	 */
	bool bWarningTriggered = false;

	/**
	 * UpdateTimer를 반복 호출하는 TimerHandle.
	 */
	FTimerHandle TimerUpdateHandle;

	/**
	 * 남은 시간을 갱신할 간격.
	 *
	 * 0.05초면 초당 20회 갱신된다.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Battle|Timer",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.01"
		)
	)
	float UpdateInterval = 0.05f;
		
};
