// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewPanel.h"

#include "CardPreviewEffectPanel.h"
#include "CardPreview_ValuePanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
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

	if (Text_SkillCost)
	{
		Text_SkillCost->SetText(FText::AsNumber(InCardData->Cost));
	}
	
	if (Text_SkillSpeed)
	{
		Text_SkillSpeed->SetText(FText::Format(NSLOCTEXT("CardPreview", "SkillSpeed", "Skill Speed : {0}"), FText::AsNumber(InCardData->CardSpeed)));
	}
	
	if (Text_SkillCoolDown)
	{
		Text_SkillCoolDown->SetText(FText::Format(NSLOCTEXT("CardPreview", "CoolDown", "Skill Speed : {0}"), FText::AsNumber(InCardData->Cooldown)));
	}

	if (Image_CardIllustration)
	{
		Image_CardIllustration->SetBrushFromTexture(InCardData->CardTexture);
	}

	if (CardPreviewEffectPanel)
	{
		CardPreviewEffectPanel->SetCardData(InCardData);
	}
	
	RefreshValuePanels(InCardData);
	
	if (CardPreviewEffectPanel)
	{
		CardPreviewEffectPanel->StartAutoScroll();
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
	
	if (CardPreviewEffectPanel)
	{
		CardPreviewEffectPanel->StopAutoScroll();
	}
}

void UCardPreviewPanel::RefreshValuePanels(const UMuksiBattleCardDataAsset* CardData)
{
	if (!VerticalBox_ValuePanels)
	{
		return;
	}

	// 이번 스킬에서 표시할 값들을 순서대로 수집
	TArray<FDescriptionValueData> Values;

	if (CardData)
	{
		for (const FCardEffectDisplayData& EffectData
			: CardData->EffectDisplayData)
		{
			for (const FDescriptionValueData& ValueData : EffectData.DescriptionValues)
			{
				if (ValueData.DescriptionType == EDescriptionValueType::None)
				{
					continue;
				}
				Values.Add(ValueData);
			}
		}
	}

	// 부족한 패널만 추가 생성
	while (ValuePanels.Num() < Values.Num())
	{
		if (!ValuePanelClass)
		{
			break;
		}

		UCardPreview_ValuePanel* NewPanel = CreateWidget<UCardPreview_ValuePanel>(GetOwningPlayer(), ValuePanelClass);

		if (!NewPanel)
		{
			break;
		}

		VerticalBox_ValuePanels->AddChildToVerticalBox(NewPanel);
		ValuePanels.Add(NewPanel);
	}

	// 필요한 패널 갱신, 남는 패널 숨김
	for (int32 i = 0; i < ValuePanels.Num(); ++i)
	{
		UCardPreview_ValuePanel* Panel = ValuePanels[i];

		if (!Panel)
		{
			continue;
		}

		if (Values.IsValidIndex(i))
		{
			Panel->SetValue(Values[i].DescriptionType, Values[i].MainValue);
			Panel->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Panel->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
