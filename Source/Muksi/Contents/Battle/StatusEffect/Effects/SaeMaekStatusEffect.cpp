// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/SaeMaekStatusEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

void USaeMaekStatusEffect::OnRoundStart()
{
	if (!IsValid(OwnerComponent))
	{
		SetStack(0);
		return;
	}

	const int32 ParalysisStack = GetCurrentStack();

	if (ParalysisStack > 0)
	{
		OwnerComponent->AddStatusEffect(
			MuksiStatusEffectIDs::Paralysis,
			ParalysisStack,
			1
		);
	}
	
}

void USaeMaekStatusEffect::OnRoundEnd()
{
	SetStack(GetCurrentStack() - 1);
}

void USaeMaekStatusEffect::OnBattleActionStart(const FBattleAction& BattleAction)
{
	//여긴 Bleed랑 똑같이 BattleAction보고 해당 카드 타입이 공격카드이면 위력만큼 체력 감소
	//일단 카드 타입은 넘어가기
	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(OwnerActor);
	if (!IsValid(OwnerCharacter))
	{
		return;
	}
	UBattleStatComponent* StatComponent =
		OwnerCharacter->GetBattleStatComponent();

	if (!IsValid(StatComponent))
	{
		return;
	}

	const int32 BleedDamage = GetCurrentStack();

	StatComponent->ApplyDamage(
		static_cast<float>(BleedDamage)
	);
}
