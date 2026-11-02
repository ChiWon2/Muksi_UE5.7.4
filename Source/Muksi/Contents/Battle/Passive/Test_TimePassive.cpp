// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/Test_TimePassive.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectIDs.h"

UTest_TimePassive::UTest_TimePassive()
{
	PassiveName = FText::FromString(TEXT("초조함"));
	Passive1();
}

void UTest_TimePassive::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	Super::HandleBattlePhaseChanged(OldPhase, NewPhase);

	if (NewPhase != EBattlePhase::RoundStart)
		return;

	ABattleCharacterBase* Character = GetOwnerCharacter();

	if (!IsValid(Character))
		return;

	UMuksiWorldManagerSubsystem* Subsystem = UMuksiWorldManagerSubsystem::Get(Character);

	if (!IsValid(Subsystem))
		return;

	ABattleManager* BattleManager = Subsystem->GetManager<ABattleManager>();

	if (!IsValid(BattleManager))
		return;

	const int32 CurrentRound = BattleManager->GetCurrentRound();
	if (CurrentRound < 1)
		return;
	
	Passive1_();
	OnPassiveActive.Broadcast(PassiveImage, PassiveName);
}

void UTest_TimePassive::Passive1()
{
	FText PassiveDescription1 = FText::FromString(TEXT("<Style color=\"Red\" underline=\"true\">국 시작 시</> 1국 이상 일 경우 동요 2 부여"));
	
	FPassiveTextLine PassiveLine;
	PassiveLine.Text.Add(PassiveDescription1);
	
	PassiveDescriptions.Add(PassiveLine);
}

void UTest_TimePassive::Passive1_()
{
	ABattleCharacterBase* Character = GetOwnerCharacter();
	UMuksiStatusEffectComponent* StatusComponent = Character->GetStatusEffectComponent();
	StatusComponent->AddStatusEffect(MuksiStatusEffectIDs::Agitation,2,1);
}
