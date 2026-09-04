// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Card/Effect/ApplyStatusEffectCardEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"

void UApplyStatusEffectCardEffect::ExecuteEffect(const FBattleCardEffectContext& Context)
{
	if (Context.ExecutionMode != EBattleExecutionMode::ActualBattle)
	{
		return;
	}

	ABattleCharacterBase* Character = nullptr;

	switch (EffectTarget)
	{
	case EBattleCardEffectTarget::Self:
		Character = Context.User;
		break;

	case EBattleCardEffectTarget::Target:
		Character = Context.Target;
		break;
	}

	if (!IsValid(Character))
	{
		return;
	}

	if (StatusEffectID.IsNone())
	{
		return;
	}

	UMuksiStatusEffectComponent* StatusEffectComponent =
		Character->GetStatusEffectComponent();

	if (!IsValid(StatusEffectComponent))
	{
		return;
	}

	StatusEffectComponent->AddStatusEffect(StatusEffectID,StackCount,Duration);
}
