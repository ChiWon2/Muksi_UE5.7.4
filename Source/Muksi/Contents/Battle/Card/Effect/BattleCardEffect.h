// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleCardEffectTypes.h"
#include "UObject/Object.h"
#include "BattleCardEffect.generated.h"
class UBattleCardEffectCondition;
struct FBattleCardEffectContext;
/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UBattleCardEffect : public UObject
{
	GENERATED_BODY()
public:
	virtual void Execute(const FBattleCardEffectContext& Context);
	virtual void ModifyDamage(FBattleDamageContext& Context);
	
	EBattleCardEffectTrigger GetTrigger() const{return Trigger;}

protected:
	bool CheckConditions(const FBattleCardEffectContext& Context) const;
	virtual void ExecuteEffect(const FBattleCardEffectContext& Context);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Card Effect")
	EBattleCardEffectTrigger Trigger = EBattleCardEffectTrigger::OnCardUsed;
	
	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Card Effect")
	TArray<TObjectPtr<UBattleCardEffectCondition>> Conditions;
};
