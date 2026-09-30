// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Test_Widget_BattleTimerProgeress.generated.h"

class UProgressBar;
class UImage;

/**
 * 
 */
UCLASS()
class MUKSI_API UTest_Widget_BattleTimerProgeress : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Timer;

	UPROPERTY(meta = (BindWidget))
	UImage* Image_TimerHandle;
	
private:
	// 전체 시간
	UPROPERTY(EditAnywhere, Category = "Timer Test")
	float TotalTime = 10.0f;

	// 현재 남은 시간
	float RemainingTime = 0.0f;

	// 타이머 실행 여부
	bool bTimerRunning = false;

	// 핸들의 가로 크기
	UPROPERTY(EditAnywhere, Category = "Timer Test")
	float HandleWidth = 40.0f;
	
private:
	void InitializeTimer();
	
	void UpdateTimerVisual(float Percent);
};
