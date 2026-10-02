// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/ActorWidget/SkillInfo/Actor_SkillInfoImage.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UActor_SkillInfoImage::SetCardData(UMuksiBattleCardDataAsset* InCardData)
{
	if (!IsValid(InCardData))
	{
		return;
	}

	CardData = InCardData;

	if (!IsValid(Image_Skill))
	{
		return;
	}

	// CardIcon이 UTexture2D*인 경우
	if (IsValid(CardData->CardTexture))
	{
		Image_Skill->SetBrushFromTexture(CardData->CardTexture);
	}
	
	if (!IsValid(TextBlock_Skill))
	{
		return;
	}
	TextBlock_Skill->SetText(CardData->CardName);
}

void UActor_SkillInfoImage::PlayShowAnimation()
{
	if (Anim_Show)
	{
		PlayAnimation(Anim_Show);
	}
}
