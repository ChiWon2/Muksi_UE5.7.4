// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Popup/EffectDescriptionPopup.h"

#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
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

void UEffectDescriptionPopup::ShowAtPosition(const FVector2D& Position)
{
	if (!Border_Center || !CanvasPanel_Root)
	{
		return;
	}

	UCanvasPanelSlot* CanvasSlot =
		Cast<UCanvasPanelSlot>(
			Border_Center->Slot
		);

	if (!CanvasSlot)
	{
		return;
	}

	// 실제 Popup 내용만 다시 표시
	Border_Center->SetVisibility(
		ESlateVisibility::HitTestInvisible
	);

	ForceLayoutPrepass();

	const FVector2D PopupSize =
		Border_Center->GetDesiredSize();

	const FVector2D ViewportSize =
		CanvasPanel_Root
		->GetCachedGeometry()
		.GetLocalSize();

	constexpr float XThreshold = 0.7f;
	constexpr float YThreshold = 0.7f;

	constexpr float OffsetX = 15.0f;
	constexpr float OffsetY = 15.0f;

	FVector2D PopupPosition;

	// X
	if (Position.X >=
		ViewportSize.X * XThreshold)
	{
		// 화면 오른쪽
		// → 마우스 왼쪽
		PopupPosition.X =
			Position.X
			- PopupSize.X
			- OffsetX;
	}
	else
	{
		// → 마우스 오른쪽
		PopupPosition.X =
			Position.X
			+ OffsetX;
	}

	// Y
	if (Position.Y >=
		ViewportSize.Y * YThreshold)
	{
		// 화면 아래
		// → 마우스 위
		PopupPosition.Y =
			Position.Y
			- PopupSize.Y
			- OffsetY;
	}
	else
	{
		// → 마우스 아래
		PopupPosition.Y =
			Position.Y
			+ OffsetY;
	}

	CanvasSlot->SetPosition(
		PopupPosition
	);
}

void UEffectDescriptionPopup::HidePopup()
{
	if (!Border_Center)
	{
		return;
	}

	Border_Center->SetVisibility(ESlateVisibility::Collapsed);
}

void UEffectDescriptionPopup::ShowByHover(FName EffectID, const FText& EffectName, const FText& Description,
                                          const FSlateBrush& IconBrush, const FVector2D& Position)
{
	SetEffectData(EffectName, Description, IconBrush);

	ShowAtPosition(Position);
}

void UEffectDescriptionPopup::HideByHover()
{
	HidePopup();
}



void UEffectDescriptionPopup::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Popup 전체는 화면을 덮지만 입력은 절대로 먹지 않음
	SetVisibility(ESlateVisibility::HitTestInvisible);

	// 실제 내용만 처음에는 숨김
	if (Border_Center)
	{
		Border_Center->SetVisibility(ESlateVisibility::Collapsed);
	}
}

