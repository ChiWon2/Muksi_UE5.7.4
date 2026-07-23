// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Status/CharacterStatusWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"

void UCharacterStatusWidget::SetData(ABattleCharacterBase* BattleCharacter)
{
	Image_CharacterImage->SetBrushFromTexture(BattleCharacter->GetCharacterData()->CharacterIllustration);
	TextBlock_CharacterName->SetText(BattleCharacter->GetCharacterData()->CharacterName);
	
	BattleStatComponent = BattleCharacter->FindComponentByClass<UBattleStatComponent>();
	if (!BattleStatComponent){UE_LOG(LogTemp, Error, TEXT("BattleStatComponent not found (CharacterStatusWidget.cpp	)")); return;}
	MaxHP = BattleStatComponent->GetMaxHP();
	HPChanged(MaxHP, MaxHP);
	BattleStatComponent->OnHPChanged.AddDynamic(this, &UCharacterStatusWidget::HPChanged);
}

void UCharacterStatusWidget::HPChanged(float PreHP, float AftHP)
{
	if (AftHP - PreHP >= 0)
	{
		HPUp(PreHP, AftHP);
	}else
	{
		HPDown(PreHP, AftHP);
	}
}

void UCharacterStatusWidget::HPUp(float PreHP, float AftHP)
{
	const float HPPercent =
		MaxHP > 0.0f
		? FMath::Clamp(AftHP / MaxHP, 0.0f, 1.0f)
		: 0.0f;

	if (IsValid(ProgressBar_CharacterHP))
	{
		ProgressBar_CharacterHP->SetPercent(HPPercent);
	}

	if (IsValid(TextBlock_CharacterHP))
	{
		TextBlock_CharacterHP->SetText(
			FText::Format(
				NSLOCTEXT(
					"BattleUI",
					"CharacterHPFormat",
					"{0} / {1}"
				),
				FText::AsNumber(FMath::RoundToInt(AftHP)),
				FText::AsNumber(FMath::RoundToInt(MaxHP))
			)
		);
	}
}

void UCharacterStatusWidget::HPDown(float PreHP, float AftHP)
{
	const float HPPercent =
		MaxHP > 0.0f
		? FMath::Clamp(AftHP / MaxHP, 0.0f, 1.0f)
		: 0.0f;

	if (IsValid(ProgressBar_CharacterHP))
	{
		ProgressBar_CharacterHP->SetPercent(HPPercent);
	}

	if (IsValid(TextBlock_CharacterHP))
	{
		TextBlock_CharacterHP->SetText(
			FText::Format(
				NSLOCTEXT(
					"BattleUI",
					"CharacterHPFormat",
					"{0} / {1}"
				),
				FText::AsNumber(FMath::RoundToInt(AftHP)),
				FText::AsNumber(FMath::RoundToInt(MaxHP))
			)
		);
	}
}


