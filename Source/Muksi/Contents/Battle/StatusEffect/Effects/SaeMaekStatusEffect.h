// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "SaeMaekStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API USaeMaekStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()
public:
	virtual void OnRoundStart() override;
	virtual void OnRoundEnd() override;
	virtual void OnBattleActionStart(const FBattleAction& BattleAction) override;
	
};
