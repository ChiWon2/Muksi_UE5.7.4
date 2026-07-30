// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

#include "Muksi/Contents/Battle/BattleManager.h"

void UCharacterPassive::BeginDestroy()
{
	if (CachedBattleManager)
	{
		CachedBattleManager->OnBattlePhaseChanged.RemoveDynamic(this, &UCharacterPassive::HandleBattlePhaseChanged);
		CachedBattleManager = nullptr;
	}

	Super::BeginDestroy();
}

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner)
{
	OwnerCharacter = InOwner;
}


void UCharacterPassive::BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	if (CachedBattleManager)
	{
		CachedBattleManager->OnBattlePhaseChanged.RemoveDynamic(this, &UCharacterPassive::HandleBattlePhaseChanged);
	}

	CachedBattleManager = BattleManager;

	if (CachedBattleManager)
	{
		CachedBattleManager->OnBattlePhaseChanged.AddUniqueDynamic(this, &UCharacterPassive::HandleBattlePhaseChanged);
	}
}

void UCharacterPassive::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (!bEnabled)
	{
		return;
	}

	OnBattlePhaseChanged(OldPhase, NewPhase);
}

void UCharacterPassive::OnBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
}
