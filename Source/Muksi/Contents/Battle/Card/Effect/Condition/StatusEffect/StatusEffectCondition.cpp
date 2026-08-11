// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Card/Effect/Condition/StatusEffect/StatusEffectCondition.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

bool UStatusEffectCondition::CheckCondition(const FBattleCardEffectContext& Context) const
{
	ABattleCharacterBase* Character = nullptr;

	switch (ConditionTarget)
	{
	case EBattleCardConditionTarget::Self:
		Character = Context.User;
		break;

	case EBattleCardConditionTarget::Target:
		Character = Context.Target;
		break;
	}

	if (!IsValid(Character))
	{
		return false;
	}

	UMuksiStatusEffectComponent* StatusEffectComponent =
		Character->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
	{
		return false;
	}
	
	const int32 CurrentStack = StatusEffectComponent->GetEffectStackCount(StatusEffectID);

	switch (CompareType)
	{
	case EBattleCardCompareType::Equal:
		return CurrentStack == CompareStack;

	case EBattleCardCompareType::Greater:
		return CurrentStack > CompareStack;

	case EBattleCardCompareType::GreaterEqual:
		return CurrentStack >= CompareStack;

	case EBattleCardCompareType::Less:
		return CurrentStack < CompareStack;

	case EBattleCardCompareType::LessEqual:
		return CurrentStack <= CompareStack;
	}

	return false;
}
