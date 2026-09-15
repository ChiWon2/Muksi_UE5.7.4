// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewEffectPanel.h"

#include "CardPreviewEffectEntry.h"
#include "Components/RichTextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UCardPreviewEffectPanel::SetCardData(UMuksiBattleCardDataAsset* InCardData)
{
	if (!RichText_DeceiveInfo)
	{
		return;
	}
	// 기본은 숨김
	RichText_DeceiveInfo->SetVisibility(ESlateVisibility::Collapsed);
	
	if (!InCardData)
	{
		ClearEffectData();
		return;
	}
	
	const TArray<FCardEffectDisplayData>& DisplayData = InCardData->EffectDisplayData;

	// 필요한 Entry 개수 확보
	EnsureEntryCount(DisplayData.Num());

	for (int32 Index = 0; Index < EffectEntries.Num(); ++Index)
	{
		UCardPreviewEffectEntry* Entry = EffectEntries[Index];

		if (!Entry)
		{
			continue;
		}

		if (DisplayData.IsValidIndex(Index))
		{
			// 기존 Entry에 새로운 카드 정보만 덮어쓰기
			Entry->SetEffectData(DisplayData[Index]);

			Entry->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			// 이번 카드에서는 필요 없는 Entry
			Entry->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	
	// 변초 카드가 아니면 그대로 종료
	if (!InCardData->bIsDeceiveCard)
	{
		return;
	}
	
	const FString DeceiveText = FString::Printf(
		TEXT("<Deceive>[변초]: </><CardName>%s</>"),
		*InCardData->ActualCard->CardName.ToString()
	);

	RichText_DeceiveInfo->SetText(FText::FromString(DeceiveText));
	RichText_DeceiveInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UCardPreviewEffectPanel::ClearEffectData()
{
	for (UCardPreviewEffectEntry* Entry : EffectEntries)
	{
		if (Entry)
		{
			Entry->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UCardPreviewEffectPanel::EnsureEntryCount(int32 RequiredCount)
{
	if (!EffectVerticalBox || !EffectEntryClass)
	{
		return;
	}

	while (EffectEntries.Num() < RequiredCount)
	{
		UCardPreviewEffectEntry* NewEntry = CreateWidget<UCardPreviewEffectEntry>(this, EffectEntryClass);

		if (!NewEntry)
		{
			return;
		}

		UVerticalBoxSlot* EffectSlot = EffectVerticalBox->AddChildToVerticalBox(NewEntry);
		if (EffectSlot)
		{
			EffectSlot->SetPadding(FMargin(0.f, 10.f, 0.f, 4.f));
		}
		EffectEntries.Add(NewEntry);
	}
}
