// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Muksi/Widgets/Battle/Popup/EffectDescriptionPopup.h"

void UEffectRichTextBlock::ShowEffectDescription(const FText& EffectName, const FEffectKeywordStyle& EffectStyle)
{
	if (!EffectDescriptionPopupClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EffectDescriptionPopupClass가 설정되지 않았습니다.")
		);

		return;
	}

	if (!EffectDescriptionPopup)
	{
		APlayerController* PlayerController =
			GetOwningPlayer();

		if (!PlayerController)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("EffectRichTextBlock의 OwningPlayer가 없습니다.")
			);

			return;
		}

		EffectDescriptionPopup =
			CreateWidget<UEffectDescriptionPopup>(
				PlayerController,
				EffectDescriptionPopupClass
			);

		if (!EffectDescriptionPopup)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("효과 설명 팝업 생성 실패")
			);

			return;
		}

		EffectDescriptionPopup->AddToViewport(1000);

		// 전달한 화면 좌표가 팝업 왼쪽 아래가 되도록 설정
		EffectDescriptionPopup->SetAlignmentInViewport(
			FVector2D(0.0f, 1.0f)
		);
	}

	EffectDescriptionPopup->SetEffectData(
		EffectName,
		EffectStyle.Description,
		EffectStyle.IconBrush
	);

	const FVector2D MousePosition =
		UWidgetLayoutLibrary::GetMousePositionOnViewport(this);

	const FVector2D PopupOffset(15.0f, -15.0f);

	EffectDescriptionPopup->SetPositionInViewport(
		MousePosition + PopupOffset,
		false
	);

	EffectDescriptionPopup->SetVisibility(
		ESlateVisibility::Visible
	);
}

void UEffectRichTextBlock::HideEffectDescription()
{
	if (EffectDescriptionPopup)
	{
		EffectDescriptionPopup->SetVisibility(
			ESlateVisibility::Collapsed
		);
	}
}
