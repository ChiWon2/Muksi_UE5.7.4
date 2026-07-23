// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner)
{
	OwnerCharacter = InOwner;
}


void UCharacterPassive::BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	//그냥 직접 바인딩 하는 방식으로 가자
}
