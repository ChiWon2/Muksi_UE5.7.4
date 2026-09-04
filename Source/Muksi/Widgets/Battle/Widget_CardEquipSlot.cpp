// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_CardEquipSlot.h"

#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Animation/WidgetAnimation.h"


FReply UWidget_CardEquipSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bSlotEnabled && bPlayerSlot && EquippedCard)
		{
			OnCardUnequipRequested.Broadcast(this);
			return FReply::Handled();
		}
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UWidget_CardEquipSlot::IsPointInsideSlot(const FVector2D& ScreenPosition) const
{
	if (!SlotBorder)
	{
		return false;
	}

	const FGeometry& Geometry = SlotBorder->GetCachedGeometry();
	const FVector2D LocalPos = Geometry.AbsoluteToLocal(ScreenPosition);

	return LocalPos.X >= 0.f && LocalPos.Y >= 0.f &&
		LocalPos.X <= Geometry.GetLocalSize().X &&
		LocalPos.Y <= Geometry.GetLocalSize().Y;
}

bool UWidget_CardEquipSlot::EquipCard(UWidget_BattleCardBase* InCard)
{
	if (!CanEquipCard(InCard))
	{
		return false;
	}
	
	UMuksiBattleCardDataAsset* CardData = InCard->GetCardData();
	if (!CardData)
	{
		return false;
	}
	
	if (!CardHostOverlay)
	{
		return false;
	}
	
	EquippedCard = InCard;
	SlotData.CardInstanceId = InCard->GetCardInstanceId();
	SlotData.CardData = CardData;

	InCard->RemoveFromParent();

	UOverlaySlot* OverlaySlot = CardHostOverlay->AddChildToOverlay(InCard);
	if (OverlaySlot)
	{
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}
	InCard->SetVisibility(ESlateVisibility::HitTestInvisible);

	RefreshSlotVisual();
	
	return true;
}

bool UWidget_CardEquipSlot::EquipCard_Enemy(UWidget_BattleCardBase* InCard)
{
	if (!InCard)
	{
		return false;
	}
	if (EquippedCard)
	{
		return false;
	}
	
	UMuksiBattleCardDataAsset* CardData = InCard->GetCardData();
	if (!CardData)
	{
		return false;
	}
	
	if (!CardHostOverlay)
	{
		return false;
	}
	
	EquippedCard = InCard;
	SlotData.CardData = CardData;

	InCard->RemoveFromParent();

	UOverlaySlot* OverlaySlot = CardHostOverlay->AddChildToOverlay(InCard);
	if (OverlaySlot)
	{
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}
	InCard->SetVisibility(ESlateVisibility::HitTestInvisible);

	RefreshSlotVisual();
	
	return true;
}

bool UWidget_CardEquipSlot::CanEquipCard(UWidget_BattleCardBase* InCard) const
{
	if (!InCard)
	{
		return false;
	}

	if (EquippedCard)
	{
		return false;
	}

	if (SlotData.bConfirmed || !bSlotEnabled)
	{
		return false;
	}

	if (!InCard->GetCardData())
	{
		return false;
	}

	if (!CardHostOverlay)
	{
		return false;
	}

	return true;
}

UWidget_BattleCardBase* UWidget_CardEquipSlot::ReleaseCard()
{
	if (!EquippedCard)
	{
		return nullptr;
	}

	UWidget_BattleCardBase* CardToReturn = EquippedCard;

	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.bConfirmed = false;
	SlotData.CardInstanceId.Invalidate();

	bHighlighted = false;

	CardToReturn->RemoveFromParent();

	RefreshSlotVisual();

	return CardToReturn;
}





FVector2D UWidget_CardEquipSlot::GetSlotSize() const
{
	return GetCachedGeometry().GetLocalSize();	
}

bool UWidget_CardEquipSlot::IsCardOverlappingSlot(UWidget_BattleCardBase* InCard) const
{
	if (!InCard)
	{
		return false;
	}

	const FGeometry& CardGeometry = InCard->GetCachedGeometry();
	const FGeometry& SlotGeometry = GetCachedGeometry();

	const FVector2D CardAbsPos = CardGeometry.GetAbsolutePosition();
	const FVector2D CardSize = CardGeometry.GetLocalSize() * CardGeometry.Scale;

	const FVector2D SlotAbsPos = SlotGeometry.GetAbsolutePosition();
	const FVector2D SlotSize = SlotGeometry.GetLocalSize() * SlotGeometry.Scale;

	const FVector2D CardCenter = CardAbsPos + CardSize * 0.5f;

	const bool bInsideX =
		CardCenter.X >= SlotAbsPos.X &&
		CardCenter.X <= SlotAbsPos.X + SlotSize.X;

	const bool bInsideY =
		CardCenter.Y >= SlotAbsPos.Y &&
		CardCenter.Y <= SlotAbsPos.Y + SlotSize.Y;

	return bInsideX && bInsideY;
}

bool UWidget_CardEquipSlot::ClearEquipSlot()
{
	if (!EquippedCard)
	{
		return false;
	}

	/*
	 * 주의:
	 * RemoveFromParent()는 카드 위젯을 화면에서 제거한다.
	 * 카드를 다시 손패로 되돌리는 구조라면 RemoveFromParent()를 쓰면 안 될 수 있다.
	 */
	EquippedCard->RemoveFromParent();
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.CardInstanceId.Invalidate();

	RefreshSlotVisual();

	return true;
}

bool UWidget_CardEquipSlot::CheckEmptySlot()
{
	if (EquippedCard)return false;
	return true;
}



void UWidget_CardEquipSlot::SetSlotInfo(int32 InSlotIndex, int32 InExchangeNumber)
{
	SlotData.SlotIndex = InSlotIndex;
	SlotData.ExchangeNumber = InExchangeNumber;

	RefreshSlotVisual();
}




void UWidget_CardEquipSlot::ClearSlot()
{
	if (SlotData.bConfirmed)
	{
		//UE_LOG(LogTemp, Warning, TEXT("ClearSlot ignored: slot already confirmed"));
		return;
	}
	//if (EquippedCard)EquippedCard->RemoveFromParent(); //빼는 애니메이션 있으면 넣기
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.CardInstanceId.Invalidate();

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::ForceClearSlot()
{
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.bConfirmed = false;
	SlotData.CardInstanceId.Invalidate();

	bSlotEnabled = false;
	bHighlighted = false;

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::ConfirmSlot()
{
	SlotData.bConfirmed = true;
	bSlotEnabled = false;
	bHighlighted = false;
	
	
	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::SetSlotEnabled(bool bEnabled)
{
	if (SlotData.bConfirmed && bEnabled)
	{
		UE_LOG(LogTemp, Log, TEXT("Test"));
		bSlotEnabled = false;
	}
	else
	{
		bSlotEnabled = bEnabled;
	}

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::SetSlotHighlighted(bool bInHighlighted)
{
	if (SlotData.bConfirmed)
	{
		bHighlighted = false;
	}
	else
	{
		bHighlighted = bInHighlighted;
	}

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::SetSlotConfirmed(bool bConfirmed)
{
	SlotData.bConfirmed = bConfirmed;

	if (bConfirmed)
	{
		bSlotEnabled = false;
		bHighlighted = false;
	}

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::RefreshSlotVisual()
{
	if (SlotBorder)
	{
		if (SlotData.bConfirmed)
		{
			SlotBorder->SetRenderOpacity(0.8f);
		}
		else if (bHighlighted)
		{
			SlotBorder->SetRenderOpacity(1.0f);
		}
		else if (bSlotEnabled)
		{
			SlotBorder->SetRenderOpacity(0.9f);
		}
		else
		{
			SlotBorder->SetRenderOpacity(0.4f);
		}
	}
}

void UWidget_CardEquipSlot::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);
	
	if (!bWaitingForTurnOrderTiltFinish
		|| Animation != ActiveTurnOrderAnimation.Get())
	{
		return;
	}

	bWaitingForTurnOrderTiltFinish = false;
	ActiveTurnOrderAnimation = nullptr;

	OnTurnOrderTiltFinished.Broadcast(this);
}

bool UWidget_CardEquipSlot::PlayTurnOrderTilt(bool bFirst)
{
	bWaitingForTurnOrderTiltFinish = false;
	ActiveTurnOrderAnimation = nullptr;

	StopTurnOrderAnimations();

	UWidgetAnimation* TurnOrderAnimation = nullptr;

	if (bPlayerSlot)
	{
		TurnOrderAnimation =
			bFirst
				? Anim_TurnorderTilit_First.Get()
				: Anim_TurnorderTilit_Second.Get();
	}
	else
	{
		TurnOrderAnimation =
			bFirst
				? Anim_TurnorderTilit_First_Enemy.Get()
				: Anim_TurnorderTilit_Second_Enemy.Get();
	}

	if (!TurnOrderAnimation)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[CardEquipSlot] Turn order animation is missing. "
				"PlayerSlot=%s First=%s Exchange=%d"),
			bPlayerSlot ? TEXT("true") : TEXT("false"),
			bFirst ? TEXT("true") : TEXT("false"),
			SlotData.ExchangeNumber);

		return false;
	}

	ActiveTurnOrderAnimation = TurnOrderAnimation;
	bWaitingForTurnOrderTiltFinish = true;

	PlayAnimation(TurnOrderAnimation);

	return true;
}

void UWidget_CardEquipSlot::PlayTurnOrderReset()
{
	bWaitingForTurnOrderTiltFinish = false;
	ActiveTurnOrderAnimation = nullptr;

	StopTurnOrderAnimations();

	UWidgetAnimation* ResetAnimation =
		bPlayerSlot
			? Anim_TurnorderTilit_Reset.Get()
			: Anim_TurnorderTilit_Reset_Enemy.Get();

	if (!ResetAnimation)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[CardEquipSlot] Turn order reset animation "
				"is missing. PlayerSlot=%s Exchange=%d"),
			bPlayerSlot ? TEXT("true") : TEXT("false"),
			SlotData.ExchangeNumber);

		return;
	}

	PlayAnimation(ResetAnimation);
}

void UWidget_CardEquipSlot::StopTurnOrderAnimations()
{
	const TArray<UWidgetAnimation*> TurnOrderAnimations =
	{
		Anim_TurnorderTilit_First.Get(),
		Anim_TurnorderTilit_Second.Get(),
		Anim_TurnorderTilit_Reset.Get(),

		Anim_TurnorderTilit_First_Enemy.Get(),
		Anim_TurnorderTilit_Second_Enemy.Get(),
		Anim_TurnorderTilit_Reset_Enemy.Get()
	};

	for (UWidgetAnimation* Animation : TurnOrderAnimations)
	{
		if (Animation)
		{
			StopAnimation(Animation);
		}
	}
}

