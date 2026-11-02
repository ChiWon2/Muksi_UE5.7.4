// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerProfilePanel.generated.h"

struct FBattleCharacterInfo;

class ABattleCharacterBase;
class UMuksiCharacterDataAsset;
class UImage;
class UTextBlock;
class UProgressBar;
class UVerticalCommonTextBlock;
class UCharacterBattleStatPanel;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_PlayerProfilePanel : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void SetBattleCharacter(ABattleCharacterBase* BattleCharacter){PlayerCharacter = BattleCharacter;};
	void SetData(const FBattleCharacterInfo& Info);
	//void SetData(UMuksiCharacterDataAsset* DataAsset);

protected:
	//***** BindWidget *****	Begin
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_PlayerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_PlayerIntroduction;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> CharacterHPProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterBattleStatPanel> CharacterBattleStatPanel;
	//***** BindWidget *****	End
	
	void SetHP(float CurrentHP);
	UPROPERTY()
	float MaxHP = 0.f;
	
	TObjectPtr<ABattleCharacterBase> PlayerCharacter = nullptr;
	
	UFUNCTION()
	void HPChanged(float PreHP, float AftHP);

	void HPUp(float PreHP, float AftHP);
	void HPDown(float PreHP, float AftHP);
	

	
};
