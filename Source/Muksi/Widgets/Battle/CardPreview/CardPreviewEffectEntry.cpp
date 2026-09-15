// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewEffectEntry.h"

#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDisplayData.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"


void UCardPreviewEffectEntry::SetEffectData(const FCardEffectDisplayData& InEffectData)
{
	//공격 관련 타입이 전부 정해지면 그때
	//SetAttackTypeIcon(InEffectData.AttackType);

	if (Text_Value)
	{
		Text_Value->SetText(FText::AsNumber(InEffectData.AttackValue));
	}

	if (Text_EffectDescription)
	{
		Text_EffectDescription->SetHoverPopupEnabled(false);
		Text_EffectDescription->SetText(InEffectData.EffectDescription);
	}
}

/*void UCardPreviewEffectEntry::SetAttackTypeIcon(EMuksiAttackCardType AttackType)
{
	/*UTexture2D* IconTexture = nullptr;

	switch (AttackType)
	{
	case EMuksiAttackCardType::Normal:
		IconTexture = NormalAttackIcon;
		break;

	case EMuksiAttackCardType::Rush:
		IconTexture = RushAttackIcon;
		break;

	case EMuksiAttackCardType::RangeAttack:
		IconTexture = RangeAttackIcon;
		break;

	default:
		break;
	}

	if (Image_AttackType && IconTexture)
	{
		Image_AttackType->SetBrushFromTexture(IconTexture);
	}#1#
}*/
