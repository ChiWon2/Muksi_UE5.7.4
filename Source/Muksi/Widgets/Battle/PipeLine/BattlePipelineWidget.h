// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattlePipelineWidget.generated.h"

class UInkLineWidget;
DECLARE_MULTICAST_DELEGATE(FOnBattlePipelinePresentationFinished);
/**
 * 
 */
UCLASS()
class MUKSI_API UBattlePipelineWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeDestruct() override;

	//InkWidget-------------------------------------------------
public:
	void DisplayInkLine(const FString& InText, float Time);
	void DisplayInkLineEnabled(const FString& InText, float Time);
	FOnBattlePipelinePresentationFinished OnPresentationFinished;
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInkLineWidget> InkLineWidget;
	void DisplayInkLineDisabled();
private:
	void HandleInkLineFinished();
	FTimerHandle InkLineTimerHandle;
	//---------------------------------------------------------
};
