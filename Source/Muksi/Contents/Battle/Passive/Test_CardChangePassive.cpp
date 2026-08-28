// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_CardChangePassive.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Widgets/Battle/Hand/Card/BattleCardManager.h"

UTest_CardChangePassive::UTest_CardChangePassive()
{
	PassiveName = FText::FromString(TEXT("테스트카드변경"));
}

void UTest_CardChangePassive::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (NewPhase != EBattlePhase::RoundStart)
	{
		return;
	}
	Passive1();
}

void UTest_CardChangePassive::Passive1()
{
	ABattleCharacterBase* Owner = GetOwnerCharacter();
	if (!IsValid(Owner))
	{
		return;
	}

	UMuksiWorldManagerSubsystem* ManagerSubsystem =
		UMuksiWorldManagerSubsystem::Get(Owner);

	if (!IsValid(ManagerSubsystem))
	{
		return;
	}

	ABattleManager* BattleManager =
		ManagerSubsystem->GetManager<ABattleManager>();

	if (!IsValid(BattleManager))
	{
		return;
	}

	UBattleCardManager* CardManager =
		BattleManager->GetBattleCardManager();

	if (!IsValid(CardManager))
	{
		return;
	}
	UBattleCardComponent* CardComponent =
		OwnerCharacter->GetBattleCardComponent();
	
	const FBattleCardInstance* FoundCard =
		CardComponent->FindHandCardByData(ACardDataAsset);
	
	if (!FoundCard)
	{
		return;
	}
	
	CardManager->ReplaceHandCard(
		OwnerCharacter,
		FoundCard->InstanceId,
		BCardDataAsset);
	
	//패시브 작동 UI 띄우기
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
}

void UTest_CardChangePassive::Passive1Setting()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 카드를 변경합니다."));
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	PassiveDescriptions.Add(PassiveLine);
}
