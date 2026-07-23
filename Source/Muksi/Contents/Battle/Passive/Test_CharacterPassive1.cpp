// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_CharacterPassive1.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

UTest_CharacterPassive1::UTest_CharacterPassive1()
{
	PassiveName = FText::FromString(TEXT("운기조식"));
	
	Passive1();
	Passive2();
}

void UTest_CharacterPassive1::BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	Super::BindingEvent(BattleManager, BattleMainScreen);
	BattleManager->OnRoundEnded.AddDynamic(this, &UTest_CharacterPassive1::Passive1_);
	BattleManager->OnRoundStarted.AddDynamic(this, &UTest_CharacterPassive1::Passive2_);
}



void UTest_CharacterPassive1::Passive1()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 종료 시</> 체력을 2 회복합니다."));
	FText PassiveDescription2 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 종료 시</> <Effect id=\"Bleed\">출혈</> 2 부여")); 
	
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	PassiveLine.Text.Add(PassiveDescription2);
	
	PassiveDescriptions.Add(PassiveLine);
}

void UTest_CharacterPassive1::Passive1_()
{
	//대충 실행 시킬 바인딩함수
	
	//지금 당장 힐 이펙트가 없으니까
	ABattleCharacterBase* Character = GetOwnerCharacter();
	Character->SetCurrentHP(Character->GetCurrentHP() + 2);
	
	//패시브 작동 UI 띄우기
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
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
	Character->SetCurrentHP(Character->GetCurrentHP() + 2);
	//나중에는 막 버프 넣기 같은 거 넣을 수 도 있음
	
	//패시브 작동 UI 띄우기
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
}
