// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "BattlePipelineWidget.generated.h"

enum class EBattlePhase : uint8;
class UInkLineWidget;
DECLARE_MULTICAST_DELEGATE(FOnBattlePipelinePresentationFinished);
/**
 * 
 */
USTRUCT(BlueprintType)
struct FBattlePhasePresentationContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	EBattlePhase Phase = EBattlePhase::BattleStart;
	
	UPROPERTY(BlueprintReadOnly)
	int32 RoundNum = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeNum = 0;//일단 Exchange 수, Round 수 넘기는 용도. 더 많아지면 세분화
};

UCLASS()
class MUKSI_API UBattlePipelineWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeDestruct() override;
	void DisplayPhase(FBattlePhasePresentationContext& PhaseContext);
	
	FOnBattlePipelinePresentationFinished OnPresentationFinished;
protected:
	void HandleBattlePipelineFinished();
	
	//InkWidget-------------------------------------------------
protected:
	void DisplayInkLine(const FString& InText, float Time);//Widget_BattleMainScreen에서 기다려줬으면 하는 InkLine
	void DisplayInkLineEnabled(const FString& InText, float Time);//Widget_BattleMainScreen에서 기다려주지 않아도 되는 InkLine
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UInkLineWidget> InkLineWidget;
	void DisplayInkLineDisabled();
	
	int32 HandleBattlePipelineFinishCount = 0;
	UPROPERTY(EditAnywhere)
	float InkTime = 0.1f;
	
	UPROPERTY(EditAnywhere)
	FString BattleStartText = "Battle Start!";
	
	UPROPERTY(EditAnywhere)
	FString BattleEndText = "Battle End!";
	
	UPROPERTY(EditAnywhere)
	FString RoundStartText = "Round Start!";
	
	UPROPERTY(EditAnywhere)
	FString RoundEndText = "Round End";
	
	UPROPERTY(EditAnywhere)
	FString ExchangeStartText= "Exchange Start!";

	UPROPERTY(EditAnywhere)
	FString ExchangeCountText = "Exchange";

	UPROPERTY(EditAnywhere)
	FString ExchangeEndText = "Exchange End!";
	
	UPROPERTY(EditAnywhere)
	FString BattleActionSequenceStartText = "BattleAction Start!";
	
	UPROPERTY(EditAnywhere)
	FString BattleActionSequenceEndText = "BattleAction End!";
private:
	void HandleInkLineFinished();
	FTimerHandle InkLineTimerHandle;
	//---------------------------------------------------------
};
