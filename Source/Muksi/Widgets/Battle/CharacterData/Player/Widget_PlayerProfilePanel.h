// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerProfilePanel.generated.h"

class ABattleCharacterBase;
class UMuksiCharacterDataAsset;
class UImage;
class UTextBlock;
class USlider;
class UVerticalCommonTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_PlayerProfilePanel : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void SetBattleCharacter(ABattleCharacterBase* BattleCharacter){PlayerCharacter = BattleCharacter;};
	void SetData(UMuksiCharacterDataAsset* DataAsset);

protected:
	//***** BindWidget *****	Begin
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalCommonTextBlock> PlayerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalCommonTextBlock> PlayerIntroduction;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> PlayerHPSlider;
	//***** BindWidget *****	End
	
	void SetHP(int32 CurrentHP);
	UPROPERTY()
	float MaxHP = 0.f;
	
	TObjectPtr<ABattleCharacterBase> PlayerCharacter = nullptr;
	
	UFUNCTION()
	void HPChanged(float PreHP, float AftHP);

	void HPUp(float PreHP, float AftHP);
	void HPDown(float PreHP, float AftHP);
	
};
