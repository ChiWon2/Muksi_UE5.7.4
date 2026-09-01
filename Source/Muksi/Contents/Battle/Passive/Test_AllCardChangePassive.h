// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"
#include "Test_AllCardChangePassive.generated.h"

class UMuksiBattleCardDataAsset;
/**
 * 
 */
UCLASS()
class MUKSI_API UTest_AllCardChangePassive : public UCharacterPassive
{
	GENERATED_BODY()
public:
	UTest_AllCardChangePassive();
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
