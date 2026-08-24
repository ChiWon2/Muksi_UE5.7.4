// Fill out your copyright notice in the Description page of Project Settings.


#include "ExchangeSlotPanelWidget.h"

#include "ExchangeSlotTypes.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Muksi/Widgets/Battle/Widget_CardEquipSlot.h"

void UExchangeSlotPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	InitializeExchangeSlots();
}

void UExchangeSlotPanelWidget::NativeDestruct()
{
	for (UWidget_BattleCardBase* CardWidget : EnemySelectedBattleCards)
	{
		if (CardWidget)
		{
			CardWidget->OnCardFlipFinished.RemoveAll(this);
		}
	}
	EnemySelectedBattleCards.Empty();
	PendingEnemyCardRevealIndex = INDEX_NONE;
	Super::NativeDestruct();
}

UWidget_CardEquipSlot* UExchangeSlotPanelWidget::FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const
{
	if (!Card)
	{
		return nullptr;
	}

	for (UWidget_CardEquipSlot* EquipSlot : PlayerExchangeSlots)
	{
		if (!EquipSlot)
		{
			continue;
		}

		if (!EquipSlot->IsCardOverlappingSlot(Card))
		{
			continue;
		}

		if (!EquipSlot->CheckEmptySlot())
		{
			return nullptr;
		}

		return EquipSlot;
	}

	return nullptr;
}

UWidget_CardEquipSlot* UExchangeSlotPanelWidget::GetPlayerSlotByExchangeNumber(int32 ExchangeNumber) const
{
	const int32 SlotIndex = ExchangeNumber - 1;

	if (!PlayerExchangeSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Invalid Player ExchangeNumber: %d"),
			ExchangeNumber
		);

		return nullptr;
	}

	return PlayerExchangeSlots[SlotIndex];
}

FCardEquipSlotData UExchangeSlotPanelWidget::GetSlotDataByExchangeNumber_Player(int32 InIndex)
{
	const int32 SlotIndex = InIndex - 1;

	if (!PlayerExchangeSlots.IsValidIndex(SlotIndex))
	{
		return FCardEquipSlotData();
	}

	if (!PlayerExchangeSlots[SlotIndex])
	{
		return FCardEquipSlotData();
	}

	return PlayerExchangeSlots[SlotIndex]->GetSlotData();
}

FCardEquipSlotData UExchangeSlotPanelWidget::GetSlotDataByExchangeNumber_Enemy(int32 InIndex)
{
	const int32 SlotIndex = InIndex - 1;

	if (!EnemyExchangeSlots.IsValidIndex(SlotIndex))
	{
		return FCardEquipSlotData();
	}

	if (!EnemyExchangeSlots[SlotIndex])
	{
		return FCardEquipSlotData();
	}

	return EnemyExchangeSlots[SlotIndex]->GetSlotData();
}


void UExchangeSlotPanelWidget::ConfirmExchangeInput(int32 InIndex)
{
	if (InIndex >= 3)
	{
		for (UWidget_CardEquipSlot* EquipSlot : PlayerExchangeSlots)
		{
			if (!EquipSlot)
			{
				continue;
			}

			EquipSlot->ConfirmSlot();
			EquipSlot->SetSlotEnabled(false);
			EquipSlot->SetSlotHighlighted(false);
			EquipSlot->SetSlotConfirmed(false);
		}
	}
	else
	{
		UWidget_CardEquipSlot* EquipSlot =
			GetPlayerSlotByExchangeNumber(InIndex);

		if (!EquipSlot)
		{
			return;
		}

		EquipSlot->ConfirmSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(true);

		UWidget_CardEquipSlot* NextSlot =
			GetPlayerSlotByExchangeNumber(InIndex + 1);

		if (!NextSlot)
		{
			return;
		}

		NextSlot->SetSlotEnabled(true);
		NextSlot->SetSlotHighlighted(true);
		NextSlot->SetSlotConfirmed(false);
	}
}


void UExchangeSlotPanelWidget::EnableExchangeSlots(int32 InIndex)
{
	const int32 ActiveSlotIndex = InIndex - 1;

	if (!PlayerExchangeSlots.IsValidIndex(ActiveSlotIndex))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EnableExchangeSlots failed: invalid ExchangeNumber %d"),
			InIndex
		);

		return;
	}

	for (int32 i = 0; i < PlayerExchangeSlots.Num(); ++i)
	{
		UWidget_CardEquipSlot* EquipSlot =
			PlayerExchangeSlots[i];

		if (!EquipSlot)
		{
			continue;
		}

		const bool bActive = (i == ActiveSlotIndex);

		EquipSlot->SetSlotEnabled(bActive);
		EquipSlot->SetSlotHighlighted(bActive);

		if (bActive)
		{
			EquipSlot->SetSlotConfirmed(false);
		}
	}
}

void UExchangeSlotPanelWidget::EnableExchangeSlot(int32 InIndex, bool bActive)
{
	const int32 SlotIndex = InIndex - 1;

	if (!PlayerExchangeSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EnableExchangeSlot failed: %d"),
			InIndex
		);

		return;
	}

	UWidget_CardEquipSlot* EquipSlot =
		PlayerExchangeSlots[SlotIndex];

	if (!EquipSlot)
	{
		return;
	}

	EquipSlot->SetSlotEnabled(bActive);
	EquipSlot->SetSlotHighlighted(bActive);
}

bool UExchangeSlotPanelWidget::EnemySelectedBattleCardFlip(int32 InIndex, bool bFront)
{
	if (!EnemySelectedBattleCards.IsValidIndex(InIndex)
		|| !EnemySelectedBattleCards[InIndex])
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EnemySelectedBattleCardFlip failed: invalid exchange index %d"),
			InIndex
		);

		return false;
	}

	UWidget_BattleCardBase* CardWidget =
		EnemySelectedBattleCards[InIndex];

	if (!bFront)
	{
		CardWidget->PlayCardFlipToBack();
		return true;
	}

	PendingEnemyCardRevealIndex = InIndex;

	CardWidget->OnCardFlipFinished.RemoveAll(this);

	CardWidget->OnCardFlipFinished.AddUObject(
		this,
		&UExchangeSlotPanelWidget::HandleEnemySelectedCardFlipFinished
	);

	CardWidget->PlayCardFlipToFront();

	return true;
}

void UExchangeSlotPanelWidget::PlaceEnemySelectCard(UMuksiBattleCardDataAsset* SelectCard, int32 ExchangeCount)
{
	if (!SelectCard || !EnemyExchangeSlots.IsValidIndex(ExchangeCount) || !EnemyExchangeSlots[ExchangeCount])
	{
		UE_LOG(LogTemp, Warning, TEXT("PlaceEnemySelectCard failed: invalid exchange index %d"), ExchangeCount);
		return;
	}

	UWidget_BattleCardBase* CardWidget = CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);

	if (!CardWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: CreateWidget failed (HandWidget.cpp)"));
		return;
	}
	CardWidget->SetCardData_(SelectCard, false);

	CardWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	EnemyExchangeSlots[ExchangeCount]->SetVisibility(ESlateVisibility::Visible);
	EnemyExchangeSlots[ExchangeCount]->EquipCard_Enemy(CardWidget);

	//TODO CardWidget 뒤집는 애니메이션
	//EnemySelectCardVerticalBox->AddChildToVerticalBox(CardWidget);
	EnemySelectedBattleCards.SetNum(EnemyExchangeSlots.Num());
	EnemySelectedBattleCards[ExchangeCount] = CardWidget;
}

TArray<FReleasedExchangeCard> UExchangeSlotPanelWidget::ReleasePlayerCards()
{
	TArray<FReleasedExchangeCard> ReleasedCards;

	ForceLayoutPrepass();

	for (UWidget_CardEquipSlot* EquipSlot : PlayerExchangeSlots)
	{
		if (!EquipSlot)
		{
			continue;
		}

		UWidget_BattleCardBase* CardWidget =
			EquipSlot->GetEquipSlot();

		if (!CardWidget)
		{
			EquipSlot->ForceClearSlot();
			continue;
		}

		const FGeometry& CardGeometry =
			CardWidget->GetCachedGeometry();

		FReleasedExchangeCard ReleasedCard;

		ReleasedCard.CardWidget = CardWidget;

		ReleasedCard.AbsoluteBottomCenter =
			CardGeometry.LocalToAbsolute(
				FVector2D(
					CardGeometry.GetLocalSize().X * 0.5f,
					CardGeometry.GetLocalSize().Y
				)
			);

		EquipSlot->ForceClearSlot();
		CardWidget->RemoveFromParent();

		ReleasedCards.Add(ReleasedCard);
	}

	return ReleasedCards;
}

TArray<FReleasedExchangeCard> UExchangeSlotPanelWidget::ReleaseEnemyCards()
{
	TArray<FReleasedExchangeCard> ReleasedCards;

	ForceLayoutPrepass();

	for (UWidget_CardEquipSlot* EquipSlot : EnemyExchangeSlots)
	{
		if (!EquipSlot)
		{
			continue;
		}

		UWidget_BattleCardBase* CardWidget = EquipSlot->GetEquipSlot();

		if (!CardWidget)
		{
			EquipSlot->ForceClearSlot();
			continue;
		}

		const FGeometry& CardGeometry = CardWidget->GetCachedGeometry();

		FReleasedExchangeCard ReleasedCard;

		ReleasedCard.CardWidget = CardWidget;

		ReleasedCard.AbsoluteBottomCenter =
			CardGeometry.LocalToAbsolute(
				FVector2D(
					CardGeometry.GetLocalSize().X * 0.5f,
					CardGeometry.GetLocalSize().Y
				)
			);

		EquipSlot->ForceClearSlot();
		CardWidget->RemoveFromParent();

		ReleasedCards.Add(ReleasedCard);
	}

	EnemySelectedBattleCards.Empty();
	PendingEnemyCardRevealIndex = INDEX_NONE;
	return ReleasedCards;
}


void UExchangeSlotPanelWidget::HandleEnemySelectedCardFlipFinished(UWidget_BattleCardBase* CardWidget)
{
	if (PendingEnemyCardRevealIndex == INDEX_NONE)
	{
		return;
	}

	if (!EnemySelectedBattleCards.IsValidIndex(
		PendingEnemyCardRevealIndex))
	{
		PendingEnemyCardRevealIndex = INDEX_NONE;
		return;
	}

	if (EnemySelectedBattleCards[PendingEnemyCardRevealIndex]
		!= CardWidget)
	{
		return;
	}

	const int32 FinishedExchangeIndex = PendingEnemyCardRevealIndex;

	PendingEnemyCardRevealIndex = INDEX_NONE;

	CardWidget->OnCardFlipFinished.RemoveAll(this);

	OnEnemyCardRevealFinished.Broadcast(FinishedExchangeIndex);
}

void UExchangeSlotPanelWidget::InitializeExchangeSlots()
{
	PlayerExchangeSlots.Empty();

	PlayerExchangeSlots.Add(PlayerExchangeSlot_1);
	PlayerExchangeSlots.Add(PlayerExchangeSlot_2);
	PlayerExchangeSlots.Add(PlayerExchangeSlot_3);

	for (int32 i = 0; i < PlayerExchangeSlots.Num(); ++i)
	{
		UWidget_CardEquipSlot* EquipSlot = PlayerExchangeSlots[i];

		if (!EquipSlot)
		{
			continue;
		}

		const int32 SlotIndex = i;
		const int32 ExchangeNumber = i + 1;

		EquipSlot->bPlayerSlot = true;

		EquipSlot->SetSlotInfo(
			SlotIndex,
			ExchangeNumber
		);

		EquipSlot->ClearSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(false);
		EquipSlot->OnCardUnequipRequested.AddUObject(this, &UExchangeSlotPanelWidget::HandleCardUnequipRequested);
	}

	EnemyExchangeSlots.Empty();

	EnemyExchangeSlots.Add(EnemyExchangeSlot_1);
	EnemyExchangeSlots.Add(EnemyExchangeSlot_2);
	EnemyExchangeSlots.Add(EnemyExchangeSlot_3);

	for (int32 i = 0; i < EnemyExchangeSlots.Num(); ++i)
	{
		UWidget_CardEquipSlot* EquipSlot = EnemyExchangeSlots[i];

		if (!EquipSlot)
		{
			continue;
		}

		const int32 SlotIndex = i;
		const int32 ExchangeNumber = i + 1;

		EquipSlot->bPlayerSlot = false;

		EquipSlot->SetSlotInfo(
			SlotIndex,
			ExchangeNumber
		);

		EquipSlot->ClearSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(false);
	}
}

void UExchangeSlotPanelWidget::HandleCardUnequipRequested(UWidget_CardEquipSlot* EquipSlot)
{
	if (!EquipSlot)
	{
		return;
	}

	UWidget_BattleCardBase* CardWidget = EquipSlot->ReleaseCard();

	if (!CardWidget)
	{
		return;
	}

	OnCardReturnRequested.Broadcast(CardWidget);
}
