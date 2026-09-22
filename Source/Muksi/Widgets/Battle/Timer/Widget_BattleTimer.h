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
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Timer")
	void HideTimer();

protected:
	//*** BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> RemainingTimeText = nullptr;
	//*** BindWidget

private:
	float CurrentTotalDuration = 0.0f;
	float CurrentWarningTime = 0.0f;
	
	//UI Material 전용 타이머
	
protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> BottleTimerImage = nullptr;
	
	UPROPERTY(EditAnywhere, Category = "Battle|Timer|Material")
	FLinearColor StartColor =  FLinearColor(0.05f, 0.5f, 5.0f, 1.0f);// 형광 파랑

	UPROPERTY(EditAnywhere, Category = "Battle|Timer|Material")
	FLinearColor MiddleColor = FLinearColor(5.0f, 5.0f, 0.1f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Battle|Timer|Material")
	FLinearColor EndColor = FLinearColor(5.0f, 0.1f, 0.05f, 1.0f);
private:
	void UpdateTimerMaterial(float RemainingRatio);
	
	FLinearColor GetTimerColor(float ElapsedRatio) const;

private:
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> TimerMaterial = nullptr;

	// Material의 Min 최대값
	UPROPERTY(EditAnywhere, Category = "Battle|Timer|Material")
	float MaxMinValue = 0.85f;
	
};
