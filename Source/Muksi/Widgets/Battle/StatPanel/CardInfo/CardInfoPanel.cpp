// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/CardInfoPanel.h"

#include "EffectRichTextBlock.h"
#include "RichTextBlockWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/Widget_DeckCardEntry.h"

void UCardInfoPanel::SetCardData(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData ||
		!CardEntry ||
		!EffectVerticalBox)
	{
		return;
	}
	
	CardEntry->SetCardDataAsset(CardData);
	if (!TextBlock_CardName)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("TextBlock이 설정되지 않았습니다.")
		);

		return;
	}
	TextBlock_CardName->SetText(CardData->CardName);
	
	if (!RichTextBlockWidgetClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("RichTextBlockWidgetClass가 설정되지 않았습니다.")
		);

		return;
	}
	
	for (FText EffectText : CardData->CardEffectsDescription)
	{
		URichTextBlockWidget* EffectEntry =
			CreateWidget<URichTextBlockWidget>(
				GetOwningPlayer(),
				RichTextBlockWidgetClass
			);
		if (!EffectEntry)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("RichTextBlockWidget 생성 실패")
			);

			continue;
		}
		EffectEntry->SetText(EffectText);

		
		
		UVerticalBoxSlot* VerticalSlot =
			EffectVerticalBox->AddChildToVerticalBox(
				EffectEntry
			);

		if (VerticalSlot)
		{
			VerticalSlot->SetPadding(
				FMargin(0.0f, 5.0f, 0.0f, 8.0f)
			);

			VerticalSlot->SetHorizontalAlignment(
				HAlign_Fill
			);

			VerticalSlot->SetVerticalAlignment(
				VAlign_Center
			);
		}
		
	}
}
