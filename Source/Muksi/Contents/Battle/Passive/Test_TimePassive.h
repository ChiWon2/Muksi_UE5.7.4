// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Test_TimePassive.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UTest_TimePassive : public UCharacterPassive
{
	GENERATED_BODY()
	
public:
	UTest_TimePassive();
	
protected:
	virtual void HandleBattlePhaseChanged(
		EBattlePhase OldPhase,
		EBattlePhase NewPhase) override;
	
	UFUNCTION()
	void Passive1();
	UFUNCTION()
	void Passive1_();
};
