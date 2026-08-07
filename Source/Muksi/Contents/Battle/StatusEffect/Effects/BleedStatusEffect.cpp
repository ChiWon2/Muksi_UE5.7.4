// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/Effects/BleedStatusEffect.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"


void UBleedStatusEffect::OnBattleActionStart(const FBattleAction& BattleAction)
{
	//BattleAction에서 공격 카드 타입 확인하는건 패스
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

void UBleedStatusEffect::OnRoundEnd()
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[Bleed] RoundEnd | Removed")
	);

	/*
	 * 일반 출혈은 국 소모 시 전부 사라진다.
	 * Stack이 0이 되면 StatusEffectComponent가
	 * RemoveExpiredEffects()에서 제거한다.
	 */
	SetStack(0);
}
