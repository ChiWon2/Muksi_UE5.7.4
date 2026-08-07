// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_RemoveHealth.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

UTest_RemoveHealth::UTest_RemoveHealth()
{
	PassiveName = FText::FromString(TEXT("자해"));
	
	Passive1();
}

void UTest_RemoveHealth::BindingEvent(ABattleManager* BattleManager, UWidget_BattleMainScreen* BattleMainScreen)
{
	Super::BindingEvent(BattleManager, BattleMainScreen);
	/*UE_LOG(LogTemp, Error, TEXT("RemoveHealth Binding "));
	BattleManager->OnBattlePhaseChanged
	BattleManager->OnExchangeStarted.AddDynamic(this, &UTest_RemoveHealth::Passive1_);*/
}

void UTest_RemoveHealth::OnBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	Super::OnBattlePhaseChanged(OldPhase, NewPhase);
	switch (NewPhase)
	{
	case EBattlePhase::ExchangeStart:
		Passive1_();
		break;
	default:
		break;
	}
}

void UTest_RemoveHealth::Passive1()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">합 시작 시</> 체력을 2 감소합니다."));
	FText PassiveDescription2 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 종료 시</> <Effect id=\"Bleed\">출혈</> 2 부여")); 
	
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
	Character->SetCurrentHP(Character->GetCurrentHP() - 2);
}
