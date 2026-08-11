// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"
#include "ApplyStatusEffectCardEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UApplyStatusEffectCardEffect : public UBattleCardEffect
{
	GENERATED_BODY()
protected:
	virtual void ExecuteEffect(const FBattleCardEffectContext& Context) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EBattleCardEffectTarget EffectTarget = EBattleCardEffectTarget::Target;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	FName StatusEffectID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect", meta = (ClampMin = "1"))
	int32 StackCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect", meta = (ClampMin = "1"))
	int32 Duration = 1;
};
