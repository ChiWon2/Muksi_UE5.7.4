// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/StatusEffect/SelectTimeModifierStatusEffect.h"
#include "ComposureStatusEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UComposureStatusEffect : public USelectTimeModifierStatusEffect
{
	GENERATED_BODY()
public:
	UComposureStatusEffect()
	{
		SecondsPerStack = 1.0f;
	}
};
