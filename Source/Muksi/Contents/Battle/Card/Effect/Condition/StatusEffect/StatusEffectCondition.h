// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Card/Effect/Condition/BattleCardEffectCondition.h"
#include "Muksi/Contents/Battle/Data/BattleCardEffectTypes.h"
#include "StatusEffectCondition.generated.h"

/**
 * 
 */

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UStatusEffectCondition : public UBattleCardEffectCondition
{
	GENERATED_BODY()
public:
	virtual bool CheckCondition(const FBattleCardEffectContext& Context) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	EBattleCardConditionTarget ConditionTarget = EBattleCardConditionTarget::Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	FName StatusEffectID = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition")
	EBattleCardCompareType CompareType = EBattleCardCompareType::GreaterEqual;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Condition", meta = (ClampMin = "0"))
	int32 CompareStack = 1;
};
