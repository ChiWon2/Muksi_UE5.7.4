// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/CardInfoEffectEntry.h"

#include "EffectRichTextBlock.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDisplayData.h"

void UCardInfoEffectEntry::SetEffectData(const FCardEffectDisplayData& InEffectData)
{
	if (Text_AttackValue)
	{
		Text_AttackValue->SetText(FText::AsNumber(InEffectData.AttackValue));
	}

	if (Text_EffectDescription)
	{
		Text_EffectDescription->SetText(InEffectData.EffectDescription);
	}
}
