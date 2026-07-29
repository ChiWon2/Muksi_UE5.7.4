// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleTimer.generated.h"

class UProgressBar;
class UTextBlock;
class UCanvasPanel;
class UImage;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleTimer : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void ShowTimer(
		float TotalDuration,
		float WarningTime
	);

	//남은 시간 텍스트/프로그래스 바
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void UpdateTimerDisplay(
		float RemainingTime,
		float RemainingRatio
	);

	//경고 연출
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StartWarning();

	//시간 종료 연출
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void ExpireTimer();
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void HideTimer();

protected:
	//*** BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> TimerProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> RopeBackgroundImage = nullptr;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> RopeBurnImage = nullptr;
	//*** BindWidget

	UPROPERTY(
		Transient,
		meta = (BindWidgetAnimOptional)
	)
	TObjectPtr<UWidgetAnimation> WarningAnimation = nullptr;

	UPROPERTY(
		Transient,
		meta = (BindWidgetAnimOptional)
	)
	TObjectPtr<UWidgetAnimation> ExpiredAnimation = nullptr;

private:
	float CurrentTotalDuration = 0.0f;
	float CurrentWarningTime = 0.0f;
};
