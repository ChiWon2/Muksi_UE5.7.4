// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_CharacterPassive1.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

UTest_CharacterPassive1::UTest_CharacterPassive1()
{
	PassiveName = FText::FromString(TEXT("운기조식"));
	
	Passive1();
	Passive2();
}

void UTest_CharacterPassive1::HandleBattlePhaseChanged(EBattlePhase OldPhase,EBattlePhase NewPhase)
{
	Super::HandleBattlePhaseChanged(OldPhase, NewPhase);

	switch (NewPhase)
	{
	case EBattlePhase::RoundStart:
		Passive2_();
		Passive1_();
		OnPassiveActive.Broadcast(PassiveImage, PassiveName);
		break;
	

	default:
		break;
	}
}

void UTest_CharacterPassive1::Passive1()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 체력을 2 회복합니다."));
	
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	
	PassiveDescriptions.Add(PassiveLine);
}

void UTest_CharacterPassive1::Passive1_()
{
	//대충 실행 시킬 바인딩함수
	
	//지금 당장 힐 이펙트가 없으니까
	ABattleCharacterBase* Character = GetOwnerCharacter();
	Character->GetBattleStatComponent()->ApplyHPChange(2, TEXT("Heal"));
	
	/*if (UMuksiStatusEffectComponent* StatusEffectComponent = Character->GetStatusEffectComponent())
	{
		StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Bleed,2,  // 부여할 스택
			1   // Duration
		);
	}*/
	
}

void UTest_CharacterPassive1::Passive2()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 체력을 2 회복합니다."));
	FText PassiveDescription2 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> <Effect id=\"Bleed\">출혈</> 2 부여")); 
	
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	PassiveLine.Text.Add(PassiveDescription2);
	
	PassiveDescriptions.Add(PassiveLine);
}

void UTest_CharacterPassive1::Passive2_()
{
	//대충 실행 시킬 바인딩함수
	
	//지금 당장 힐 이펙트가 없으니까
	ABattleCharacterBase* Character = GetOwnerCharacter();
	Character->GetBattleStatComponent()->ApplyHPChange(2, TEXT("Heal"));
	//나중에는 막 버프 넣기 같은 거 넣을 수 도 있음
	
	if (UMuksiStatusEffectComponent* StatusEffectComponent = Character->GetStatusEffectComponent())
	{
		StatusEffectComponent->AddStatusEffect(MuksiStatusEffectIDs::Bleed,2,  // 부여할 스택
			1   // Duration
		);
	}
}
