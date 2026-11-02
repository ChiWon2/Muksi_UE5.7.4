// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterBattleStatPanel.generated.h"

class UTextBlock;
class ABattleCharacterBase;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterBattleStatPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetData(ABattleCharacterBase* InCharacter);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Attack;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Defense;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Speed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_MaxSkillCost;
	
};
