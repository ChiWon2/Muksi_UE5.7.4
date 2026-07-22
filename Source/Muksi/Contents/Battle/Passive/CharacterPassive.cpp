// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner)
{
	
}

bool UCharacterPassive::CanActivatePassive(EMuksiPassiveTriggerType InTriggerType,
	const FMuksiPassiveContext& Context) const
{
	return true;
}

void UCharacterPassive::ActivatePassive(EMuksiPassiveTriggerType InTriggerType, FMuksiPassiveContext& Context)
{
	
}

void UCharacterPassive::BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	//그냥 직접 바인딩 하는 방식으로 가자
}
