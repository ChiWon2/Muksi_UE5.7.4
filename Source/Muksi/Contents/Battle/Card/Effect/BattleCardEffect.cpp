// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Card/Effect/BattleCardEffect.h"

#include "Condition/BattleCardEffectCondition.h"


void UBattleCardEffect::Execute(const FBattleCardEffectContext& Context)
{
	if (!CheckConditions(Context))
	{
		return;
	}

	ExecuteEffect(Context);
}

void UBattleCardEffect::ModifyDamage(FBattleDamageContext& Context)
{
	
}

bool UBattleCardEffect::CheckConditions(const FBattleCardEffectContext& Context) const
{
	for (UBattleCardEffectCondition* Condition : Conditions)
	{
		if (!IsValid(Condition))
		{
			continue;
		}

		if (!Condition->CheckCondition(Context))
		{
			return false;
		}
	}

	return true;
}

void UBattleCardEffect::ExecuteEffect(const FBattleCardEffectContext& Context)
{
	
}
