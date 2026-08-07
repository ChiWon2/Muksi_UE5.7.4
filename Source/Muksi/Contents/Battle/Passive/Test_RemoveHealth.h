// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Test_RemoveHealth.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UTest_RemoveHealth : public UCharacterPassive
{
	GENERATED_BODY()
public:
	UTest_RemoveHealth();
	virtual void BindingEvent(ABattleManager* BattleManager) override;
	
protected:
	UFUNCTION()
	void Passive1();
	UFUNCTION()
	void Passive1_();
};
