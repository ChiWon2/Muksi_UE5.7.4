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
class UMaterialInstanceDynamic;

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
	void ShowTimer(float TotalDuration);

	//남은 시간 텍스트/프로그래스 바
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void UpdateTimerDisplay(float RemainingTime, float RemainingRatio);

	//경고 연출
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void StartWarning();

	//시간 종료 연출
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void ExpireTimer();


protected:
	//*** BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Timer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Timer|Progress")
	FLinearColor ProgressStartColor = FLinearColor(0.15f, 0.65f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Timer|Progress")
	FLinearColor ProgressEndColor = FLinearColor(1.0f, 0.08f, 0.05f, 1.0f);
	//*** BindWidget

private:
	float CurrentTotalDuration = 0.0f;
	float CurrentWarningTime = 0.0f;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> TimerMaterial = nullptr;

	// Material의 Min 최대값
	UPROPERTY(EditAnywhere, Category = "Battle|Timer|Material")
	float MaxMinValue = 0.7f;
	
};
