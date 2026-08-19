// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffect.h"
#include "PreBleedStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UPreBleedStatusEffect : public UMuksiStatusEffect
{
	GENERATED_BODY()
public:
	virtual void BuildPhaseExecutions(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutions) override;
};
