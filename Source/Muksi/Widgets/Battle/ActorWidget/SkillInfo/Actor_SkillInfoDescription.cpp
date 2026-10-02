// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoDescription.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"

void UActor_SkillInfoDescription::SetCardData(UMuksiBattleCardDataAsset* CardData)
{
	if (!IsValid(CardData) || !IsValid(Text_EffectDescription))
	{
		return;
	}
	
	TArray<FText> EffectDescriptions;

	for (const FCardEffectDisplayData& EffectData : CardData->EffectDisplayData)
	{
		if (EffectData.EffectDescription.IsEmpty())
		{
			continue;
		}

		EffectDescriptions.Add(EffectData.EffectDescription);
	}

	const FText CombinedDescription =
		FText::Join(
			FText::FromString(TEXT("\n")),
			EffectDescriptions
		);

	Text_EffectDescription->SetText(CombinedDescription);
}

void UActor_SkillInfoDescription::PlayShowAnimation()
{
	if (Anim_Show)
	{
		PlayAnimation(Anim_Show);
	}
}
