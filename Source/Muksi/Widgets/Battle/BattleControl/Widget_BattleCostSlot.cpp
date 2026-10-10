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

void UWidget_BattleCostSlot::SetState(EBattleCostSlotState State)
{
	if (!Image_Fill)
	{
		return;
	}

	Image_Fill->SetVisibility(
		State == EBattleCostSlotState::Empty
			? ESlateVisibility::Hidden
			: ESlateVisibility::HitTestInvisible
	);

	const FLinearColor Color =
		State == EBattleCostSlotState::PreviewConsume
			? FLinearColor(1.0f, 0.1f, 0.08f, 1.0f)
			: FLinearColor(0.0f, 0.65f, 1.0f, 1.0f);

	Image_Fill->SetColorAndOpacity(Color);
}
