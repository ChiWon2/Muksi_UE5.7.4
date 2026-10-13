// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewEffectEntry.h"

#include "Components/RichTextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDisplayData.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"


void UCardPreviewEffectEntry::SetEffectData(const FCardEffectDisplayData& InEffectData)
{
	if (Text_EffectDescription)
	{
		Text_EffectDescription->SetHoverPopupEnabled(false);
		Text_EffectDescription->SetText(InEffectData.EffectDescription);
	}
}


