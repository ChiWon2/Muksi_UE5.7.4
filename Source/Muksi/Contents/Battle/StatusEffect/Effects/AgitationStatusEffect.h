// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/SelectTimeModifierStatusEffect.h"
#include "AgitationStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UAgitationStatusEffect : public USelectTimeModifierStatusEffect
{
	GENERATED_BODY()
	
public:
	UAgitationStatusEffect()
	{
		SecondsPerStack = -1.0f;
	}
};
