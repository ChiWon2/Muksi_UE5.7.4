// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_CardEquipSlot.h"

#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "WorldPartition/ContentBundle/ContentBundleLog.h"


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

void UWidget_CardEquipSlot::EquipCard(UWidget_BattleCardBase* InCard)
{
	if (!InCard)
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("Equip card"));
	EquippedCard = InCard;
}

FVector2D UWidget_CardEquipSlot::GetSlotCenterInHandCanvas() const
{
	if (const UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(this->Slot))
	{
		const FVector2D SlotPos = CanvasPanelSlot->GetPosition();
		const FVector2D SlotSize = GetCachedGeometry().GetLocalSize();

		return SlotPos + (SlotSize * 0.5f);
	}

	return FVector2D::ZeroVector;
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

	const UCanvasPanelSlot* CardSlot = Cast<UCanvasPanelSlot>(InCard->Slot);
	const UCanvasPanelSlot* EquipSlot = Cast<UCanvasPanelSlot>(this->Slot);

	if (!CardSlot || !EquipSlot)
	{
		return false;
	}

	const FVector2D CardSize = InCard->GetCachedGeometry().GetLocalSize();
	const FVector2D CardCenter = CardSlot->GetPosition() + (CardSize * 0.5f);

	const FVector2D SlotPos = EquipSlot->GetPosition();
	const FVector2D SlotSize = GetCachedGeometry().GetLocalSize();

	const bool bInsideX = CardCenter.X >= SlotPos.X && CardCenter.X <= SlotPos.X + SlotSize.X;
	const bool bInsideY = CardCenter.Y >= SlotPos.Y && CardCenter.Y <= SlotPos.Y + SlotSize.Y;

	return bInsideX && bInsideY;
}

bool UWidget_CardEquipSlot::ClearEquipSlot()
{
	if (!EquippedCard)
	{
		return false;
	}

	EquippedCard->RemoveFromParent();
	EquippedCard = nullptr;

	return true;
}
