// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_CardEquipSlot.h"

#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"
#include "Muksi/Widgets/Battle/HandWidget.h"


FReply UWidget_CardEquipSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipSlot clicked"));
		
		if (bSlotEnabled && bPlayerSlot)
		{
			// 장착 카드가 있으면 핸드로 되돌리기
			UnequipCard(OwningHandWidget);

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
	if (!InCard)
	{
		return false;
	}
	if (EquippedCard)
	{
		return false;
	}
	if (!bSlotEnabled && bPlayerSlot){UE_LOG(LogTemp, Log, TEXT("EquipCard4"));}
	if (SlotData.bConfirmed){UE_LOG(LogTemp, Log, TEXT("EquipCard5"));}
	if (SlotData.bConfirmed || !bSlotEnabled)
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


FVector2D UWidget_CardEquipSlot::GetSlotCenterInHandCanvas(UHandWidget* InHandWidget) const
{
	if (!InHandWidget){return FVector2D::ZeroVector;}
	
	const FGeometry& SlotGeometry = GetCachedGeometry();
	
	const FVector2D SlotLocalCenter = SlotGeometry.GetLocalSize() * 0.5f;
	
	const FVector2D SlotAbsoluteCenter = SlotGeometry.LocalToAbsolute(SlotLocalCenter);
	
	const FGeometry& HandCanvasGeometry = InHandWidget->GetHandCanvasGeometry();
	
	return HandCanvasGeometry.AbsoluteToLocal(SlotAbsoluteCenter);
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

void UWidget_CardEquipSlot::EquipCardData(UMuksiBattleCardDataAsset* InCardData, ABattleCharacterBase* InSourceCharacter)
{
	if (SlotData.bConfirmed)
	{
		//UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: slot already confirmed"));
		return;
	}

	if (!bSlotEnabled)
	{
		//UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: slot is not enabled"));
		return;
	}

	if (!InCardData)
	{
		//UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: CardData is null"));
		return;
	}

	if (!InSourceCharacter)
	{
		//UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: SourceCharacter is null"));
		return;
	}

	SlotData.CardData = InCardData;
	SlotData.SourceCharacter = InSourceCharacter;

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::SetBattleContext(ABattleCharacterBase* InSourceCharacter)
{
	SlotData.SourceCharacter = InSourceCharacter;

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

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::ForceClearSlot()
{
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.bConfirmed = false;

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

bool UWidget_CardEquipSlot::UnequipCard(UHandWidget* HandWidget)
{
	if (!EquippedCard || !HandWidget){return false;}
	
	UWidget_BattleCardBase* CardToReturn = EquippedCard;
	
	EquippedCard = nullptr;
	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	
	CardToReturn->SetCardRenderAngle(0.0f);
	
	CardToReturn->SetVisibility(ESlateVisibility::Visible);
	HandWidget->PlaceCardInHand(CardToReturn);
	HandWidget->OrganizeCards(HandWidget->GetDefaultCardSpacing());
	
	RefreshSlotVisual();
	
	return true;
}
