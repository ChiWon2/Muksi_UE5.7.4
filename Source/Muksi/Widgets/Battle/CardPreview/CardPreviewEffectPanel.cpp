// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreviewEffectPanel.h"

#include "CardPreviewEffectEntry.h"
#include "Components/RichTextBlock.h"
#include "Components/ScrollBox.h"
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
		*InCardData->CardName.ToString()
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

void UCardPreviewEffectPanel::StartAutoScroll()
{
	bWaitingAtBottom = false;
	AutoScrollOffset = 0.0f;
	ScrollDelayRemaining = ScrollStartDelay;
	bAutoScrollActive = true;

	if (ScrollBox_Effects)
	{
		ScrollBox_Effects->SetScrollOffset(0.0f);
	}
}

void UCardPreviewEffectPanel::StopAutoScroll()
{
	bWaitingAtBottom = false;
	bAutoScrollActive = false;
	AutoScrollOffset = 0.0f;
	ScrollDelayRemaining = 0.0f;

	if (ScrollBox_Effects)
	{
		ScrollBox_Effects->SetScrollOffset(0.0f);
	}
}

void UCardPreviewEffectPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!bAutoScrollActive || !ScrollBox_Effects)
	{
		return;
	}

	float MoveDeltaTime = InDeltaTime;

	if (ScrollDelayRemaining > 0.0f)
	{
		const float WaitTime = FMath::Min(ScrollDelayRemaining, MoveDeltaTime);

		ScrollDelayRemaining -= WaitTime;
		MoveDeltaTime -= WaitTime;

		if (ScrollDelayRemaining > 0.0f)
		{
			return;
		}
	}

	// 맨 아래 대기가 끝났으면 맨 위로 복귀
	if (bWaitingAtBottom)
	{
		bWaitingAtBottom = false;
		AutoScrollOffset = 0.0f;
		ScrollDelayRemaining = ScrollStartDelay;

		ScrollBox_Effects->SetScrollOffset(0.0f);
		return;
	}

	if (MoveDeltaTime <= 0.0f)
	{
		return;
	}

	const float EndOffset = ScrollBox_Effects->GetScrollOffsetOfEnd();

	if (EndOffset <= KINDA_SMALL_NUMBER)
	{
		AutoScrollOffset = 0.0f;
		ScrollBox_Effects->SetScrollOffset(0.0f);
		return;
	}

	AutoScrollOffset = FMath::Min(AutoScrollOffset + AutoScrollSpeed * MoveDeltaTime,EndOffset);

	ScrollBox_Effects->SetScrollOffset(AutoScrollOffset);

	// 맨 아래에 도착하면 해당 위치에서 대기
	if (AutoScrollOffset >= EndOffset)
	{
		bWaitingAtBottom = true;
		ScrollDelayRemaining = ScrollEndDelay;
	}
}
