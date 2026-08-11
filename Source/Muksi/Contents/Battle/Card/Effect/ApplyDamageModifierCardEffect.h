// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"
#include "ApplyDamageModifierCardEffect.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API UApplyDamageModifierCardEffect : public UBattleCardEffect
{
	GENERATED_BODY()
public:
	virtual void ModifyDamage(FBattleDamageContext& Context) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	FName SourceStatusEffectID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	EBattleCardEffectTarget SourceTarget =
		EBattleCardEffectTarget::Target;

	//특정 수치당 위력
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Modifier")
	float PercentPerStack = 0.0f;
};
