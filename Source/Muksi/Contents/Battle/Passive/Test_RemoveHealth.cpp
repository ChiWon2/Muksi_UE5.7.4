// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_RemoveHealth.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

UTest_RemoveHealth::UTest_RemoveHealth()
{
	PassiveName = FText::FromString(TEXT("자해"));
	
	Passive1();
}

void UTest_RemoveHealth::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	Super::HandleBattlePhaseChanged(OldPhase, NewPhase);
	switch (NewPhase)
	{
	case EBattlePhase::RoundStart:
		Passive1_();
		break;
	default:
		break;
	}
}

void UTest_RemoveHealth::Passive1()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 체력을 20 감소합니다."));
	FText PassiveDescription2 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> <Effect id=\"Bleed\">출혈</> 2 부여")); 
	
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	PassiveLine.Text.Add(PassiveDescription2);
	
	PassiveDescriptions.Add(PassiveLine);
}

void UTest_RemoveHealth::Passive1_()
{
	UE_LOG(LogTemp, Error, TEXT("RemoveHealth Binding Passive1 Setting"));
	ABattleCharacterBase* Character = GetOwnerCharacter();
	UE_LOG(LogTemp, Error, TEXT("Test Get Current HP %f"), Character->GetCurrentHP());
	Character->GetBattleStatComponent()->ApplyHPChange(-20, TEXT("PassiveDamage"));
	
	if (UMuksiStatusEffectComponent* StatusEffectComponent = Character->GetStatusEffectComponent())
	{
		StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Bleed,2,  // 부여할 스택
			1   // Duration
		);
	}
	
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
}
