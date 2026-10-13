// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterStatusWidget.generated.h"

class UImage;
class UTextBlock;
class UProgressBar;
class ABattleCharacterBase;
class UMuksiCharacterDataAsset;
class UBattleStatComponent;
class UStatusEffectBarWidget;
class UWidget_BattleCost;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(ABattleCharacterBase* BattleCharacter);
	void RefreshCost();
protected:
	virtual void NativeDestruct() override;
	//*** Bind Widget ***//
	UPROPERTY(meta = (BindWidget))
	UImage* Image_CharacterImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_CharacterName;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_CharacterHP;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_CharacterHP;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UStatusEffectBarWidget> StatusEffectBarWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleCost> BattleCostWidget;
	
	UPROPERTY()
	TObjectPtr<UBattleStatComponent> BattleStatComponent = nullptr;
	
	UPROPERTY()
	float MaxHP = 100.f;
	
	UFUNCTION()
	void HPChanged(float PreHP, float AftHP);
	
	void HPUp(float PreHP, float AftHP);
	void HPDown(float PreHP, float AftHP);
	void UnbindBattleStatComponent();
	
	//HP ProgressBar 천천히 줄어들기--------------------------------------------------------------------------------------
public:
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_CharacterHPDelayed;

	// 피격 후 붉은 바가 움직이기까지 기다리는 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP|Animation", meta = (ClampMin = "0.0"))
	float HPDelayedStartDelay = 0.2f;

	// 초당 줄어드는 비율. 0.5이면 초당 최대 체력의 50%만큼 감소
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HP|Animation", meta = (ClampMin = "0.01"))
	float HPDelayedDecreaseSpeed = 0.5f;

	float TargetHPPercent = 1.0f;
	float DelayedHPPercent = 1.0f;
	float HPDelayRemaining = 0.0f;
	//------------------------------------------------------------------------------------------------------------------
	
	//스킬 호버링 코스트 보여주기-------------------------------------------------------------------------------------------
public:
	void SetCostPreview(int32 Cost);
	void ClearCostPreview();
	//------------------------------------------------------------------------------------------------------------------
};
