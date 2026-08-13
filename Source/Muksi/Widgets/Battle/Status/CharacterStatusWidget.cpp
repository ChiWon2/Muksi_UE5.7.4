// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Status/CharacterStatusWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "Muksi/Contents/Battle/StatusEffect/Widgets/StatusEffectBarWidget.h"

void UCharacterStatusWidget::SetData(ABattleCharacterBase* BattleCharacter)
{
	UnbindBattleStatComponent();
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(nullptr);
	if (!IsValid(BattleCharacter)) return;
	if (IsValid(BattleCharacter->GetCharacterData()))
	{
		Image_CharacterImage->SetBrushFromTexture(BattleCharacter->GetCharacterData()->CharacterIllustration);
		TextBlock_CharacterName->SetText(BattleCharacter->GetCharacterData()->CharacterName);
	}
	BattleStatComponent = BattleCharacter->GetBattleStatComponent();
	if (!BattleStatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleStatComponent not found (CharacterStatusWidget.cpp)"));
		return;
	}
	MaxHP = BattleStatComponent->GetMaxHP();
	const float CurrentHP = BattleStatComponent->GetCurrentHP();
	HPChanged(CurrentHP, CurrentHP);
	BattleStatComponent->OnHPChanged.RemoveDynamic(this, &UCharacterStatusWidget::HPChanged);
	BattleStatComponent->OnHPChanged.AddDynamic(this, &UCharacterStatusWidget::HPChanged);
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(BattleCharacter->GetStatusEffectComponent());
}

void UCharacterStatusWidget::NativeDestruct()
{
	UnbindBattleStatComponent();
	if (StatusEffectBarWidget) StatusEffectBarWidget->InitWidget(nullptr);
	Super::NativeDestruct();
}

void UCharacterStatusWidget::UnbindBattleStatComponent()
{
	if (BattleStatComponent) BattleStatComponent->OnHPChanged.RemoveDynamic(this, &UCharacterStatusWidget::HPChanged);
	BattleStatComponent = nullptr;
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


