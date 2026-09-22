// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleControlPanel.generated.h"

class UWidget_BattleSkillBar;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;
class UWidget_BattleTimer;
class UCurveFloat;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillSelected, const FGuid&, UMuksiBattleCardDataAsset*);
DECLARE_MULTICAST_DELEGATE(FOnExchangeTimeExpired);
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleControlPanel : public UUserWidget
{
	GENERATED_BODY()
	//스킬 관련 기능------------------------------------------------------------------------------------------------------
public:
	void SetBattleCharacter(ABattleCharacterBase* InCharacter);
	
	void RefreshSkillSlots();
	void RefreshSkillBar();
	
	UWidget_BattleSkillBar* GetBattleSkillBar() const{ return BattleSkillBar; }
	
	FOnBattleSkillSelected OnBattleSkillSelected;

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillBar> BattleSkillBar;
	
private:
	void HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData);
	//------------------------------------------------------------------------------------------------------------------
	
	//Timer관련 기능-----------------------------------------------------------------------------------------------------
public:
	FOnExchangeTimeExpired OnExchangeTimeExpired;

	void StartExchangeTimer();
	void StartExchangeTimer(int32 ExchangeIndex);
	void StopExchangeTimer();
protected:
	virtual void NativeTick(const FGeometry& Geometry, float InDeltaTime) override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleTimer> BattleTimerWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Timer")
	TObjectPtr<UCurveFloat> ExchangeDurationCurve;
	
private:
	void UpdateExchangeTimer(float DeltaTime);
	void HandleExchangeTimerExpired();
	
	float ExchangeTotalDuration = 15.0f;
	float ExchangeRemainingTime = 0.0f;
	float ExchangeWarningTime = 5.0f;

	bool bExchangeTimerActive = false;
	bool bWarningStarted = false;
	//------------------------------------------------------------------------------------------------------------------
};
