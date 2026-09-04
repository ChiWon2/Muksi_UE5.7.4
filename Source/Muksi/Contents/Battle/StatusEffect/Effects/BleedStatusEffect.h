// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "BleedStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UBleedStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()
	
public:
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries) override;
	
};
