// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreview_ValuePanel.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardType.h"


void UCardPreview_ValuePanel::SetValue(EDescriptionValueType ValueType, int32 Value)
{
	if (TextBlock_Value)
	{
		TextBlock_Value->SetText(FText::AsNumber(Value));
	}

	if (!Image_ValueType)
	{
		return;
	}

	const TObjectPtr<UTexture2D>* Texture = TypeStyles.Find(ValueType);

	if (Texture && IsValid(Texture->Get()))
	{
		Image_ValueType->SetBrushFromTexture(Texture->Get(), false);
		Image_ValueType->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
	else
	{
		// 등록되지 않은 타입이면 이전 아이콘을 숨김
		Image_ValueType->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UCardPreview_ValuePanel::NativePreConstruct()
{
	Super::NativePreConstruct();
}
