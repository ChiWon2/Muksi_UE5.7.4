// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_AllCardChangePassive.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Widgets/Battle/Hand/Card/BattleCardManager.h"

UTest_AllCardChangePassive::UTest_AllCardChangePassive()
{
	PassiveName = FText::FromString(TEXT("테스트_모든카드변경"));
	Passive1Setting();
}

void UTest_AllCardChangePassive::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (NewPhase != EBattlePhase::RoundStart)
	{
		return;
	}
	Passive1();
}

void UTest_AllCardChangePassive::Passive1()
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
	
	bool bCheck = true;
	for (int32 i = 0; i < CardComponent->GetCurrentHandCount(); i++)
	{
		const FBattleCardInstance* FoundCard =
		CardComponent->FindHandCardByData(ACardDataAsset);
	
		if (!FoundCard)
		{
			continue;
		}
	
		CardManager->ReplaceHandCard(
			OwnerCharacter,
			FoundCard->InstanceId,
			BCardDataAsset);
		bCheck = false;
	}
	if (bCheck){return;}
	//패시브 작동 UI 띄우기
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
}

void UTest_AllCardChangePassive::Passive1Setting()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 카드를 변경합니다."));
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	PassiveDescriptions.Add(PassiveLine);
}
