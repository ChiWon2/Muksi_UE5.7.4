// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Popup/EffectDescriptionPopup.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UEffectDescriptionPopup::SetEffectData(const FText& EffectName, const FText& EffectDescription,
	const FSlateBrush& EffectIcon)
{
	if (EffectIconImage)
	{
		EffectIconImage->SetBrush(EffectIcon);
	}

	if (EffectNameText)
	{
		EffectNameText->SetText(EffectName);
	}

	if (EffectDescriptionText)
	{
		EffectDescriptionText->SetText(EffectDescription);
	}
}
