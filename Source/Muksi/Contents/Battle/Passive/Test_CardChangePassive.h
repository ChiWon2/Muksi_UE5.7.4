// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Test_CardChangePassive.generated.h"

class UMuksiBattleCardDataAsset;
/**
 * 
 */
UCLASS(Blueprintable)
class MUKSI_API UTest_CardChangePassive : public UCharacterPassive
{
	GENERATED_BODY()
	
public:
	UTest_CardChangePassive();
	
	
protected:
	virtual void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase) override;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMuksiBattleCardDataAsset> ACardDataAsset;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMuksiBattleCardDataAsset> BCardDataAsset;
	
private:
	void Passive1();
	void Passive1Setting();
};
