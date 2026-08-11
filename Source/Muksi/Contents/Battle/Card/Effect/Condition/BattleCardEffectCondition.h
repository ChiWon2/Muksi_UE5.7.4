// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleCardEffectCondition.generated.h"
struct FBattleCardEffectContext;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UBattleCardEffectCondition : public UObject
{
	GENERATED_BODY()
public:
	virtual bool CheckCondition(
		const FBattleCardEffectContext& Context
	) const;
};
