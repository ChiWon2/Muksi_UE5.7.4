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

	if (SlotData.bConfirmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCard failed: slot already confirmed"));
		return;
	}

	if (!bSlotEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCard failed: slot is not enabled"));
		return;
	}

	UMuksiBattleCardDataAsset* CardData = InCard->GetCardData();
	if (!CardData)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCard failed: CardData is null"));
		return;
	}

	EquippedCard = InCard;

	// 카드 위젯이 아는 정보는 카드 데이터까지만 저장
	SlotData.CardData = CardData;

	//UE_LOG(LogTemp, Log, TEXT("Equip card data asset: %s"), *GetNameSafe(CardData));

	RefreshSlotVisual();
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

	/*
	 * 주의:
	 * RemoveFromParent()는 카드 위젯을 화면에서 제거한다.
	 * 카드를 다시 손패로 되돌리는 구조라면 RemoveFromParent()를 쓰면 안 될 수 있다.
	 */
	EquippedCard->RemoveFromParent();
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.TargetCharacter = nullptr;

	RefreshSlotVisual();

	return true;
}

void UWidget_CardEquipSlot::SetSlotInfo(int32 InSlotIndex, int32 InExchangeNumber)
{
	SlotData.SlotIndex = InSlotIndex;
	SlotData.ExchangeNumber = InExchangeNumber;

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::EquipCardData(UMuksiBattleCardDataAsset* InCardData, UCharacterDataBase* InSourceCharacter,
	UCharacterDataBase* InTargetCharacter)
{
	if (SlotData.bConfirmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: slot already confirmed"));
		return;
	}

	if (!bSlotEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: slot is not enabled"));
		return;
	}

	if (!InCardData)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: CardData is null"));
		return;
	}

	if (!InSourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipCardData failed: SourceCharacter is null"));
		return;
	}

	SlotData.CardData = InCardData;
	SlotData.SourceCharacter = InSourceCharacter;
	SlotData.TargetCharacter = InTargetCharacter;

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::SetBattleContext(UCharacterDataBase* InSourceCharacter,
	UCharacterDataBase* InTargetCharacter)
{
	SlotData.SourceCharacter = InSourceCharacter;
	SlotData.TargetCharacter = InTargetCharacter;

	RefreshSlotVisual();
}


void UWidget_CardEquipSlot::ClearSlot()
{
	if (SlotData.bConfirmed)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClearSlot ignored: slot already confirmed"));
		return;
	}

	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.TargetCharacter = nullptr;

	RefreshSlotVisual();
}

void UWidget_CardEquipSlot::ForceClearSlot()
{
	EquippedCard = nullptr;

	SlotData.CardData = nullptr;
	SlotData.SourceCharacter = nullptr;
	SlotData.TargetCharacter = nullptr;
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
