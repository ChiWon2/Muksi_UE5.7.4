// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/CharacterBattleStatPanel.h"

#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"

void UCharacterBattleStatPanel::SetData(ABattleCharacterBase* InCharacter)
{
	UBattleStatComponent* StatComponent = IsValid(InCharacter)
		? InCharacter->GetBattleStatComponent()
		: nullptr;

	UBattleSkillComponent* SkillComponent = IsValid(InCharacter)
		? InCharacter->GetBattleSkillComponent()
		: nullptr;

	// 소수점은 최대 한 자리까지 표시
	FNumberFormattingOptions NumberFormat;
	NumberFormat.SetMinimumFractionalDigits(0);
	NumberFormat.SetMaximumFractionalDigits(1);

	const FText EmptyText = FText::FromString(TEXT("-"));

	if (TextBlock_Attack)
	{
		TextBlock_Attack->SetText(
			IsValid(StatComponent)
				? FText::AsNumber(
					StatComponent->GetAttack(), &NumberFormat)
				: EmptyText);
	}

	if (TextBlock_Defense)
	{
		TextBlock_Defense->SetText(
			IsValid(StatComponent)
				? FText::AsNumber(
					StatComponent->GetDefense(), &NumberFormat)
				: EmptyText);
	}

	if (TextBlock_Speed)
	{
		TextBlock_Speed->SetText(
			IsValid(StatComponent)
				? FText::AsNumber(
					StatComponent->GetSpeed(), &NumberFormat)
				: EmptyText);
	}

	if (TextBlock_MaxSkillCost)
	{
		TextBlock_MaxSkillCost->SetText(
			IsValid(SkillComponent)
				? FText::AsNumber(SkillComponent->GetMaxSkillCost())
				: EmptyText);
	}
}
