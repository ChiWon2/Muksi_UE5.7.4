// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CardPreview/CardPreview_ValuePanel.h"

#include "Components/TextBlock.h"


void UCardPreview_ValuePanel::SetValue(EValueType Type, int32 Value)
{
	if (!TextBlock_Value)
	{
		return;
	}

	TextBlock_Value->SetText(FText::AsNumber(Value));
}

void UCardPreview_ValuePanel::NativePreConstruct()
{
	Super::NativePreConstruct();
}
