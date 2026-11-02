// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/StatusEffect/SelectTimeModifierStatusEffect.h"

float USelectTimeModifierStatusEffect::GetSelectTimeModifierSeconds() const
{
	return SecondsPerStack * static_cast<float>(GetCurrentStack());
}

void USelectTimeModifierStatusEffect::NotifySelectTimeUsed()
{
	bAffectedSelection = true;
}

void USelectTimeModifierStatusEffect::BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase,
	TArray<FBattleExecutionEntry>& OutExecutionEntries)
{
	(void)OldPhase;
	(void)OutExecutionEntries;

	if (NewPhase != EBattlePhase::RoundEnd)
		return;

	// 실제 선택 시간에 사용되지 않았다면 유지
	if (!bAffectedSelection)
		return;

	// 스택이나 Duration과 관계없이 전체 만료
	ConsumeDuration(GetRemainingDuration());

	bAffectedSelection = false;
}


