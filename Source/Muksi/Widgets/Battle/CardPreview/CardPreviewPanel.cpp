// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewPanel.h"

#include "CardPreviewEffectPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"


void UCardPreviewPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCardPreviewPanel::HandleCardHovered(UWidget_BattleCardBase* CardWidget)
{
	if (!CardWidget)
	{
		return;
	}
	
	UMuksiBattleCardDataAsset* CardData = CardWidget->GetCardData();
	
	if (!CardData)
	{
		return;
	}

	SetCardData(CardData);

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCardPreviewPanel::HandleCardHoverEnded(UWidget_BattleCardBase* CardWidget)
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCardPreviewPanel::HandleSkillHovered(UMuksiBattleCardDataAsset* SkillData, int32 RemainingCooldown)
{
	if (!SkillData)
	{
		return;
	}

	SetCardData(SkillData);

	// 이후 쿨다운 UI에 사용
	// SkillData->Cooldown    = 최대 쿨다운
	// RemainingCooldown     = 현재 쿨다운

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCardPreviewPanel::HandleSkillHoverEnded()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UCardPreviewPanel::SetCardData(UMuksiBattleCardDataAsset* InCardData)
{
	if (!InCardData)
	{
		return;
	}

	if (CurrentCardData == InCardData)
	{
		return;
	}

	CurrentCardData = InCardData;

	if (Text_CardName)
	{
		Text_CardName->SetText(InCardData->CardName);
	}

	if (Text_CardSpeed)
	{
		Text_CardSpeed->SetText(FText::AsNumber(InCardData->CardSpeed));
	}

	if (Image_CardIllustration)
	{
		Image_CardIllustration->SetBrushFromTexture(InCardData->CardTexture);
	}

	if (CardPreviewEffectPanel)
	{
		CardPreviewEffectPanel->SetCardData(InCardData);
	}
}

void UCardPreviewPanel::ShowPreview(UMuksiBattleCardDataAsset* InCardData)
{
	if (!InCardData)
	{
		HidePreview();
		return;
	}

	SetCardData(InCardData);

	SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCardPreviewPanel::HidePreview()
{
	SetVisibility(ESlateVisibility::Collapsed);
	CurrentCardData = nullptr;
}
