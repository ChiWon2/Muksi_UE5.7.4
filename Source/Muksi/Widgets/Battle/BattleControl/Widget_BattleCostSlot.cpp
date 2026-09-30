// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleCostSlot.h"

#include "Components/Image.h"

void UWidget_BattleCostSlot::SetFilled(bool bFilled)
{
	if (!Image_Fill)
	{
		return;
	}

	Image_Fill->SetVisibility(
		bFilled
		? ESlateVisibility::HitTestInvisible
		: ESlateVisibility::Hidden
	);
}
