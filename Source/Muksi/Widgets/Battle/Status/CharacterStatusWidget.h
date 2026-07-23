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

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(ABattleCharacterBase* BattleCharacter);
protected:
	//*** Bind Widget ***//
	UPROPERTY(meta = (BindWidget))
	UImage* Image_CharacterImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_CharacterName;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_CharacterHP;
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_CharacterHP;
	
	UPROPERTY()
	TObjectPtr<UBattleStatComponent> BattleStatComponent = nullptr;
	
	UPROPERTY()
	float MaxHP = 100.f;
	
	UFUNCTION()
	void HPChanged(float PreHP, float AftHP);
	
	void HPUp(float PreHP, float AftHP);
	void HPDown(float PreHP, float AftHP);
	
};
