// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Test_CharacterPassive1.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UTest_CharacterPassive1 : public UCharacterPassive
{
	GENERATED_BODY()
public:
	UTest_CharacterPassive1();
	
	virtual void BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen) override;
	

	
protected:
	UFUNCTION()
	void Passive1();
	UFUNCTION()
	void Passive1_();
	UFUNCTION()
	void Passive2();
	UFUNCTION()
	void Passive2_();
};
