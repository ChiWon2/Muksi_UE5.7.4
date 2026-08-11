// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Card/Effect/ApplyDamageModifierCardEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

void UApplyDamageModifierCardEffect::ModifyDamage(FBattleDamageContext& Context)
{
	ABattleCharacterBase* SourceCharacter = nullptr;

	switch (SourceTarget)
	{
	case EBattleCardEffectTarget::Self:
		SourceCharacter = Context.User;
		break;

	case EBattleCardEffectTarget::Target:
		SourceCharacter = Context.Target;
		break;
	}

	if (!IsValid(SourceCharacter))
	{
		return;
	}

	UMuksiStatusEffectComponent* StatusEffectComponent = SourceCharacter->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
	{
		return;
	}

	const int32 StackCount = StatusEffectComponent->GetEffectStackCount(SourceStatusEffectID);

	const float BonusRate = StackCount * PercentPerStack;

	Context.Damage *= 1.0f + BonusRate;
}
