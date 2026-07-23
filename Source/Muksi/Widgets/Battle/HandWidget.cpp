// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/HandWidget.h"


#include "CommonButtonBase.h"
#include "Widget_CardEquipSlot.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Muksi/Widgets/Components/InkLineWidget.h"
#include "TimerManager.h"


#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "MuksiDebugHelper.h"
#include "Status/CharacterStatusWidget.h"
#include "Widgets/Battle/Widget_BattleMainScreen.h"


FWidgetCard::FWidgetCard()
{
	
}

void UHandWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	
	
	Debug::Print(TEXT("Battle Manager Settings"));
	
	BindSelectButton();

	//SpawnDefaultHandCards();
	
	InitializeExchangeSlots();
}

void UHandWidget::NativeDestruct()
{
	

	UnbindSelectButton();
	
	
	ClearHandCards();
	Super::NativeDestruct();
}

void UHandWidget::SpawnDefaultHandCards()
{
	//ClearHandCards();
	CreateTestHandCards(5);
	OrganizeCards(DefaultCardSpacing);
}

void UHandWidget::CreateTestHandCards(int32 InCount)
{
	if (!BattleCardClass || !HandCanvas)
	{
		return;
	}

	for (int32 i = 0; i < InCount; ++i)
	{
		UWidget_BattleCardBase* Widget_BattleCard =
			CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);

		if (!Widget_BattleCard)
		{
			continue;
		}

		Widget_BattleCard->SetOwningHandWidget(this);

		if (UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(Widget_BattleCard))
		{
			
			CanvasSlot->SetAutoSize(true);

			// 화면 기준 아래 중앙
			CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));

			// 카드 자신의 기준점을 아래 중앙으로
			CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));

			CanvasSlot->SetPosition(FVector2D(0.f, -50.f)); // 아래 중앙에서 살짝 위
			CanvasSlot->SetZOrder(i);
		}

		FWidgetCard WidgetCard;
		WidgetCard.Cards = Widget_BattleCard;
		WidgetCard.ZIndex = i;

		CardsStructArray.Add(WidgetCard);
		BattleCards.Add(Widget_BattleCard);
	}
}

void UHandWidget::OrganizeCards(float OffsetX)
{
	//부채꼴 손배치
	
	if (BattleCards.IsEmpty())
	{
		return;
	}

	if (!HandCanvas || !HandCardPoint)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("OrganizeCards failed: HandCanvas or HandCardPoint is null")
		);
		return;
	}
	/*
	 * HandCardPoint의 중앙 좌표를 HandCanvas 로컬 좌표로 변환
	 */
	const FGeometry& HandGeometry =
		HandCanvas->GetCachedGeometry();

	const FGeometry& HandCardPointGeometry =
		HandCardPoint->GetCachedGeometry();

	const FVector2D HandCardPointAbsoluteCenter =
		HandCardPointGeometry.LocalToAbsolute(
			HandCardPointGeometry.GetLocalSize() * 0.5f
		);

	const FVector2D HandCardPointLocalCenter =
		HandGeometry.AbsoluteToLocal(
			HandCardPointAbsoluteCenter
		);

	/*
	 * 카드의 CanvasSlot Anchor가 (0.5, 1.0)이므로
	 * HandCanvas 아래쪽 중앙이 Position (0, 0)의 기준이다.
	 */
	const FVector2D HandBottomCenter(
		HandGeometry.GetLocalSize().X * 0.5f,
		HandGeometry.GetLocalSize().Y
	);

	/*
	 * HandCanvas 아래쪽 중앙에서 HandCardPoint 오프셋
	 */
	const FVector2D FanCenterOffset =
		HandCardPointLocalCenter - HandBottomCenter;

	const int32 CardCount = BattleCards.Num();
	const float CenterIndex =
		(CardCount - 1) * 0.5f;

	const float CurveHeight = 12.0f;
	const float AnglePerCard = 7.0f;
	const float HoverSpreadOffset = 25.0f;

	int32 HoveredIndex = INDEX_NONE;

	if (HoveredCard)
	{
		HoveredIndex =
			BattleCards.IndexOfByKey(HoveredCard);
	}

	for (int32 i = 0; i < CardCount; ++i)
	{
		UWidget_BattleCardBase* CardWidget =
			BattleCards[i];

		if (!CardWidget)
		{
			continue;
		}

		const float RelativeIndex =
			i - CenterIndex;

		const bool bIsHovered =
			(CardWidget == HoveredCard);

		float AdditionalXOffset = 0.0f;

		if (HoveredIndex != INDEX_NONE)
		{
			if (i < HoveredIndex)
			{
				AdditionalXOffset =
					-HoverSpreadOffset;
			}
			else if (i > HoveredIndex)
			{
				AdditionalXOffset =
					HoverSpreadOffset;
			}
		}

		/*
		 * HandCardPoint 중심으로 한 부채꼴 상대 좌표
		 */
		const float BaseX =
			RelativeIndex * OffsetX;

		const float BaseY =
			FMath::Square(RelativeIndex) *
			CurveHeight;

		const float HoverOffsetY =
			bIsHovered ? -25.0f : 0.0f;

		/*
		 * 기존 부채꼴 위치에 HandCardPoint 중심 오프셋 추가
		 */
		const float TargetX =
			FanCenterOffset.X +
			BaseX +
			AdditionalXOffset;

		const float TargetY =
			FanCenterOffset.Y +
			BaseY +
			HoverOffsetY;

		float TargetAngle =
			RelativeIndex * AnglePerCard;

		if (bIsHovered)
		{
			TargetAngle *= 0.35f;
		}

		CardWidget->MoveToCanvasPosition(
			FVector2D(TargetX, TargetY)
		);

		CardWidget->SetCardRenderAngle(
			TargetAngle
		);

		if (UCanvasPanelSlot* CanvasSlot =
			Cast<UCanvasPanelSlot>(CardWidget->Slot))
		{
			CanvasSlot->SetZOrder(
				bIsHovered ? 999 : i
			);
		}
	}

	
	//평행 손 배치
	/*if (BattleCards.Num() == 0)
	{
		return;
	}

	const float TotalWidth = (BattleCards.Num() - 1) * OffsetX;
	const float StartX = LiteralFloatX - (TotalWidth * 0.5f);

	for (int32 i = 0; i < BattleCards.Num(); ++i)
	{
		UWidget_BattleCardBase* Widget_BattleCard = BattleCards[i];
		if (!Widget_BattleCard)
		{
			continue;
		}

		const bool bIsHovered = (Widget_BattleCard == HoveredCard);

		const float TargetX = StartX + (OffsetX * i);
		const float TargetY = LiteralFloatY + (bIsHovered ? HoverOffsetY : 0.f);

		Widget_BattleCard->MoveToCanvasPosition(FVector2D(TargetX, TargetY));

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget_BattleCard->Slot))
		{
			CanvasSlot->SetZOrder(bIsHovered ? 999 : i);
		}
	}*/
}

void UHandWidget::CreateCardMore()
{
	CreateTestHandCards(5);
	OrganizeCards(DefaultCardSpacing);
}

void UHandWidget::ClearHandCards()
{
	if (HandCanvas)
	{
		//HandCanvas->ClearChildren();
		for (TObjectPtr<UWidget_BattleCardBase> CardWidget : BattleCards)
		{
			if (CardWidget)
			{
				CardWidget->RemoveFromParent();
			}
		}
	}
	HoveredCard = nullptr;
	BattleCards.Empty();
	CardsStructArray.Empty();
}

void UHandWidget::InvisibleHandCards()
{
	if (BattleCards.Num() == 0)
	{
		ClearHandCards();
	}else
	{
		HandCardPoint = CardDownPoint;
		OrganizeCards(DefaultCardSpacing);
	}
}

void UHandWidget::VisibleHandCards()
{
	HandCardPoint = CardUpPoint;
	OrganizeCards(DefaultCardSpacing);
}

void UHandWidget::HitActiveHandCards(bool bHitActive)
{
	if (bHitActive)
	{
		for (UWidget_BattleCardBase* CardWidget : BattleCards)
		{
			CardWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}else
	{
		for (UWidget_BattleCardBase* CardWidget : BattleCards)
		{
			CardWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
	
}


void UHandWidget::SetHoveredCard(UWidget_BattleCardBase* InHoveredCard)
{
	HoveredCard = InHoveredCard;
	OrganizeCards(DefaultCardSpacing);
}

void UHandWidget::ClearHoveredCard(UWidget_BattleCardBase* InCard)
{
	if (HoveredCard == InCard)
	{
		HoveredCard = nullptr;
		OrganizeCards(DefaultCardSpacing);
	}
}

const FGeometry& UHandWidget::GetHandCanvasGeometry() const
{ return HandCanvas->GetCachedGeometry(); }

UWidget_CardEquipSlot* UHandWidget::GetEquipSlot() const
{
	return nullptr;
}

void UHandWidget::RemoveBattleCards(UWidget_BattleCardBase* InCard)
{
	BattleCards.Remove(InCard);
}

void UHandWidget::OnClickedTurnEndButton()
{
	Debug::Print(TEXT("Clicked Turn end button (HandWidget.cpp)"));
}



UWidget_CardEquipSlot* UHandWidget::FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const
{
	
	if (!Card)
	{
		return nullptr;
	}
	
	for (UWidget_CardEquipSlot* EquipSlot : ExchangeSlots)
	{
		
		if (!EquipSlot)
		{
			continue;
		}

		if (EquipSlot->IsCardOverlappingSlot(Card))
		{
			if (!EquipSlot->CheckEmptySlot())return nullptr;
			return EquipSlot;
		}
	}
	return nullptr;
}

void UHandWidget::EnemySelectedBattleCardFlip(int32 InIndex, bool bFront)
{
	if (bFront){EnemySelectedBattleCards[InIndex]->PlayCardFlipToFront();}
	else {EnemySelectedBattleCards[InIndex]->PlayCardFlipToBack();}
	
}

void UHandWidget::PlaceEnemySelectCard(UMuksiBattleCardDataAsset* SelectCard, int32 ExchangeCount)
{
	UWidget_BattleCardBase* CardWidget =
		CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);

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
	EnemySelectedBattleCards.Add(CardWidget);
}

void UHandWidget::ClearEnemySelectCard()
{
	//임시로 Vertical Box 사용
	//EnemySelectCardVerticalBox->ClearChildren();
	
	
	/*for (UWidget_CardEquipSlot* EquipSlot : EnemyExchangeSlots)
	{
		EquipSlot->ClearSlot();
	}
	EnemySelectedBattleCards.Empty();*/
	
	for (UWidget_CardEquipSlot* EquipSlot : EnemyExchangeSlots)
	{
		UWidget_BattleCardBase* CardWidget = EquipSlot->GetEquipSlot();
		if (!CardWidget)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("DetachCardFromEquipSlot failed: CardWidget is null")
			);
			return;
		}
		ForceLayoutPrepass();
		const FGeometry& CardGeometry =
		CardWidget->GetCachedGeometry();

		const FVector2D CardAbsoluteBottomCenter =
			CardGeometry.LocalToAbsolute(
				FVector2D(
					CardGeometry.GetLocalSize().X * 0.5f,
					CardGeometry.GetLocalSize().Y
				)
			);

		
		EquipSlot->ClearSlot();
		CardWidget->RemoveFromParent();

		/*
		 * HandCanvas의 직접 자식으로 다시 추가
		 */
		UCanvasPanelSlot* CardCanvasSlot =
			HandCanvas->AddChildToCanvas(CardWidget);

		if (!CardCanvasSlot)
		{
			return;
		}

		CardCanvasSlot->SetAutoSize(true);

		// 핸드 카드 좌표계와 동일하게 통일
		CardCanvasSlot->SetAnchors(
			FAnchors(0.5f, 1.0f, 0.5f, 1.0f)
		);

		CardCanvasSlot->SetAlignment(
			FVector2D(0.5f, 1.0f)
		);

		const FGeometry& HandGeometry =
			HandCanvas->GetCachedGeometry();

		/*
		 * 카드의 기존 절대 위치를 HandCanvas 로컬 좌표로 변환
		 */
		const FVector2D CardLocalPosition =
			HandGeometry.AbsoluteToLocal(
				CardAbsoluteBottomCenter
			);

		/*
		 * 카드 Anchor가 HandCanvas 아래 중앙이므로
		 * Anchor 기준 좌표로 다시 변환
		 */
		const FVector2D HandBottomCenter(
			HandGeometry.GetLocalSize().X * 0.5f,
			HandGeometry.GetLocalSize().Y
		);

		const FVector2D CardAnchorOffset =
			CardLocalPosition - HandBottomCenter;

		CardCanvasSlot->SetPosition(CardAnchorOffset);
		CardCanvasSlot->SetZOrder(10000);

		CardWidget->SetOwningHandWidget(this);
		CardWidget->SetCardRenderAngle(0.0f);

		
		if (!CardRemovePoint_Enemy)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("CardRemovePoint_Enemy is null")
			);
			return;
		}

		ForceLayoutPrepass();

		const FGeometry& RemovePointGeometry =
			CardRemovePoint_Enemy->GetCachedGeometry();

		// 제거 지점 중앙의 절대 좌표
		const FVector2D RemoveAbsolutePosition =
			RemovePointGeometry.LocalToAbsolute(
				RemovePointGeometry.GetLocalSize() * 0.5f
			);

		// 제거 지점 절대 좌표를 HandCanvas 로컬 좌표로 변환
		const FVector2D RemoveLocalPosition =
			HandGeometry.AbsoluteToLocal(
				RemoveAbsolutePosition
			);

		// 카드의 Anchor가 HandCanvas 아래 중앙이므로
		// HandCanvas 아래 중앙 기준 오프셋으로 변환
		const FVector2D RemoveAnchorOffset =
			RemoveLocalPosition - HandBottomCenter;

		// 제거 위치로 이동
		CardWidget->MoveToCanvasPosition(
			RemoveAnchorOffset
		);
		RemoveCardArray.Add(CardWidget);
	}
}

void UHandWidget::ClearPlayerSelectCard()
{
	for (UWidget_CardEquipSlot* EquipSlot : ExchangeSlots)
	{
		UWidget_BattleCardBase* CardWidget = EquipSlot->GetEquipSlot();
		if (!CardWidget)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("DetachCardFromEquipSlot failed: CardWidget is null")
			);
			return;
		}
		ForceLayoutPrepass();
		const FGeometry& CardGeometry =
		CardWidget->GetCachedGeometry();

		const FVector2D CardAbsoluteBottomCenter =
			CardGeometry.LocalToAbsolute(
				FVector2D(
					CardGeometry.GetLocalSize().X * 0.5f,
					CardGeometry.GetLocalSize().Y
				)
			);

		
		EquipSlot->ClearSlot();
		CardWidget->RemoveFromParent();

		/*
		 * HandCanvas의 직접 자식으로 다시 추가
		 */
		UCanvasPanelSlot* CardCanvasSlot =
			HandCanvas->AddChildToCanvas(CardWidget);

		if (!CardCanvasSlot)
		{
			return;
		}

		CardCanvasSlot->SetAutoSize(true);

		// 핸드 카드 좌표계와 동일하게 통일
		CardCanvasSlot->SetAnchors(
			FAnchors(0.5f, 1.0f, 0.5f, 1.0f)
		);

		CardCanvasSlot->SetAlignment(
			FVector2D(0.5f, 1.0f)
		);

		const FGeometry& HandGeometry =
			HandCanvas->GetCachedGeometry();

		/*
		 * 카드의 기존 절대 위치를 HandCanvas 로컬 좌표로 변환
		 */
		const FVector2D CardLocalPosition =
			HandGeometry.AbsoluteToLocal(
				CardAbsoluteBottomCenter
			);

		/*
		 * 카드 Anchor가 HandCanvas 아래 중앙이므로
		 * Anchor 기준 좌표로 다시 변환
		 */
		const FVector2D HandBottomCenter(
			HandGeometry.GetLocalSize().X * 0.5f,
			HandGeometry.GetLocalSize().Y
		);

		const FVector2D CardAnchorOffset =
			CardLocalPosition - HandBottomCenter;

		CardCanvasSlot->SetPosition(CardAnchorOffset);
		CardCanvasSlot->SetZOrder(10000);

		CardWidget->SetOwningHandWidget(this);
		CardWidget->SetCardRenderAngle(0.0f);

		
		if (!CardRemovePoint_Player)
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("CardRemovePoint_Player is null")
			);
			return;
		}

		ForceLayoutPrepass();

		const FGeometry& RemovePointGeometry =
			CardRemovePoint_Player->GetCachedGeometry();

		// 제거 지점 중앙의 절대 좌표
		const FVector2D RemoveAbsolutePosition =
			RemovePointGeometry.LocalToAbsolute(
				RemovePointGeometry.GetLocalSize() * 0.5f
			);

		// 제거 지점 절대 좌표를 HandCanvas 로컬 좌표로 변환
		const FVector2D RemoveLocalPosition =
			HandGeometry.AbsoluteToLocal(
				RemoveAbsolutePosition
			);

		// 카드의 Anchor가 HandCanvas 아래 중앙이므로
		// HandCanvas 아래 중앙 기준 오프셋으로 변환
		const FVector2D RemoveAnchorOffset =
			RemoveLocalPosition - HandBottomCenter;

		// 제거 위치로 이동
		CardWidget->MoveToCanvasPosition(
			RemoveAnchorOffset
		);
		RemoveCardArray.Add(CardWidget);
	}
}


void UHandWidget::ShowTurnEndButton(bool bShow)
{
	//기존
	
	
	//바뀐 버튼
	if (!Button_Select){return;}
	
	Button_Select->SetVisibility(
	bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
	);
	
	Button_Select->SetIsEnabled(bShow);
	
}

FCardEquipSlotData UHandWidget::GetSlotDataByExchangeNumber_Player(int32 InIndex)
{
	const int32 SlotIndex = InIndex - 1;

	if (!ExchangeSlots.IsValidIndex(SlotIndex))
	{
		return FCardEquipSlotData();
	}

	if (!ExchangeSlots[SlotIndex])
	{
		return FCardEquipSlotData();
	}

	return ExchangeSlots[SlotIndex]->GetSlotData();
}

FCardEquipSlotData UHandWidget::GetSlotDataByExchangeNumber_Enemy(int32 InIndex)
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

UMuksiBattleCardDataAsset* UHandWidget::GetExchangeDataIndex_Player(int32 InIndex)
{
	FCardEquipSlotData Data = GetSlotDataByExchangeNumber_Player(InIndex);
	return Data.CardData;
}

UMuksiBattleCardDataAsset* UHandWidget::GetExchangeDataIndex_Enemy(int32 InIndex)
{
	FCardEquipSlotData Data = GetSlotDataByExchangeNumber_Enemy(InIndex);
	return Data.CardData;
}

void UHandWidget::ConfirmExchangeInput(int32 InIndex)
{
	if (InIndex >= 3)
	{
		for (UWidget_CardEquipSlot* EquipSlot : ExchangeSlots)
		{
			if (!EquipSlot){continue;}
			EquipSlot->ConfirmSlot();
			EquipSlot->SetSlotEnabled(false);
			EquipSlot->SetSlotHighlighted(false);
			EquipSlot->SetSlotConfirmed(false);
			BattleMainScreen->EquipBattleCardArray[InIndex - 1] = EquipSlot->GetSlotData().CardData;
		}
	}else
	{
		UWidget_CardEquipSlot* EquipSlot = GetSlotByExchangeNumber(InIndex);

		if (!EquipSlot)
		{
			return;
		}
		EquipSlot->ConfirmSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(true);
		BattleMainScreen->EquipBattleCardArray[InIndex - 1] = EquipSlot->GetSlotData().CardData;
		
		
		UWidget_CardEquipSlot* EquipSlot_ = GetSlotByExchangeNumber(InIndex + 1);

		if (!EquipSlot)
		{
			return;
		}
		//EquipSlot_->ConfirmSlot();
		UE_LOG(LogTemp, Log, TEXT("Confirmed Equip slot"));
		EquipSlot_->SetSlotEnabled(true);
		EquipSlot_->SetSlotHighlighted(true);
		EquipSlot_->SetSlotConfirmed(false);
	}
	
	/*EquipSlot->ConfirmSlot();
	EquipSlot->SetSlotEnabled(false);
	EquipSlot->SetSlotHighlighted(false);
	EquipSlot->SetSlotConfirmed(true);*/
}

void UHandWidget::StartExchangeInput(int32 ExchangeNumber)
{
	const int32 ActiveSlotIndex = ExchangeNumber - 1;

	for (int32 i = 0; i < ExchangeSlots.Num(); i++)
	{
		UE_LOG(LogTemp, Log, TEXT("Test"));
		UWidget_CardEquipSlot* EquipSlot = ExchangeSlots[i];

		if (!EquipSlot)
		{
			continue;
		}

		const bool bActive = (i == ActiveSlotIndex);

		/*EquipSlot->SetSlotEnabled(bActive);
		EquipSlot->SetSlotHighlighted(bActive);*/
	}
}

void UHandWidget::InitializeExchangeSlots()
{
	
	
	ExchangeSlots.Empty();

	ExchangeSlots.Add(CardEquipSlot_1);
	ExchangeSlots.Add(CardEquipSlot_2);
	ExchangeSlots.Add(CardEquipSlot_3);

	for (int32 i = 0; i < ExchangeSlots.Num(); i++)
	{
		UWidget_CardEquipSlot* EquipSlot = ExchangeSlots[i];

		if (!EquipSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("ExchangeSlots[%d] is null"), i);
			continue;
		}

		const int32 SlotIndex = i;
		const int32 ExchangeNumber = i + 1;

		EquipSlot->bPlayerSlot = true;
		
		EquipSlot->SetSlotInfo(SlotIndex, ExchangeNumber);
		EquipSlot->ClearSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(false);
		EquipSlot->OwningHandWidget = this;
	}
	
	EnemyExchangeSlots.Empty();
	EnemySelectedBattleCards.Empty();
	
	EnemyExchangeSlots.Add(EnemyCardEquipSlot_1);
	EnemyExchangeSlots.Add(EnemyCardEquipSlot_2);
	EnemyExchangeSlots.Add(EnemyCardEquipSlot_3);
	
	for (int32 i = 0; i < EnemyExchangeSlots.Num(); i++)
	{
		UWidget_CardEquipSlot* EquipSlot = EnemyExchangeSlots[i];

		if (!EquipSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("EnemyExchangeSlots[%d] is null"), i);
			continue;
		}

		const int32 SlotIndex = i;
		const int32 ExchangeNumber = i + 1;

		EquipSlot->bPlayerSlot = false;
		
		EquipSlot->SetSlotInfo(SlotIndex, ExchangeNumber);
		EquipSlot->ClearSlot();
		EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(false);
		EquipSlot->OwningHandWidget = this;
	}
}

void UHandWidget::EnableExchangeSlots(int32 InIndex)
{
	const int32 ActiveSlotIndex = InIndex - 1;

	if (!ExchangeSlots.IsValidIndex(ActiveSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartExchangeInput failed: invalid ExchangeNumber %d"), InIndex);
		return;
	}

	for (int32 i = 0; i < ExchangeSlots.Num(); i++)
	{
		UWidget_CardEquipSlot* EquipSlot = ExchangeSlots[i];

		if (!EquipSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("StartExchangeInput skipped: ExchangeSlots[%d] is null"), i);
			continue;
		}

		const bool bActive = (i == ActiveSlotIndex);

		EquipSlot->SetSlotEnabled(bActive);
		EquipSlot->SetSlotHighlighted(bActive);

		// 이미 확정된 슬롯은 다시 활성화하지 않으려면 이 부분은 상황에 따라 조절
		if (bActive)
		{
			EquipSlot->SetSlotConfirmed(false);
		}
	}

	//UE_LOG(LogTemp, Log, TEXT("StartExchangeInput: ExchangeNumber=%d, ActiveSlotIndex=%d"), ExchangeNumber, ActiveSlotIndex);
}

void UHandWidget::EnableExchangeSlot(int32 InIndex, bool bActive)
{
	const int32 ActiveSlotIndex = InIndex - 1;
	UE_LOG(LogTemp, Error, TEXT("EnabledExchange Slot Test (HandWidget.cpp)"));
	
	if (!ExchangeSlots.IsValidIndex(ActiveSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("StartExchangeInput failed: invalid ExchangeNumber %d"), InIndex);
		return;
	}
	
	UWidget_CardEquipSlot* EquipSlot = ExchangeSlots[ActiveSlotIndex];
	if (!EquipSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartExchangeInput skipped: ExchangeSlots[%d] is null"), ActiveSlotIndex);
		return;
	}
	
	EquipSlot->SetSlotEnabled(bActive);
	EquipSlot->SetSlotHighlighted(bActive);
}

void UHandWidget::ActiveHandCards(bool bActive)
{
	if (bActive)
	{
		
	}else
	{
		
	}
}

void UHandWidget::BindSelectButton()
{

	
	if (!Button_Select)
	{
		UE_LOG(LogTemp, Warning, TEXT("SelectButton is null (HandWidget.cpp)"));
		return;
	}
	
	// 중복 바인딩 방지
	Button_Select->OnClicked().RemoveAll(this);
	//Button_TurnEnd->OnClicked().RemoveAll(this);
	
	Button_Select->OnClicked().AddUObject(this, &UHandWidget::HandleEndTurnButtonClicked);
	
}

void UHandWidget::UnbindSelectButton()
{
	Button_Select->OnClicked().RemoveAll(this);
}

void UHandWidget::HandleEndTurnButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("HandWidget: EndTurnButton clicked"));

	OnEndTurnRequested.Broadcast();
}

UWidget_CardEquipSlot* UHandWidget::GetSlotByExchangeNumber(int32 ExchangeNumber) const
{
	const int32 SlotIndex = ExchangeNumber - 1;

	if (!ExchangeSlots.IsValidIndex(SlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid ExchangeNumber: %d"), ExchangeNumber);
		return nullptr;
	}

	return ExchangeSlots[SlotIndex];
}

void UHandWidget::DisplayInkLine(FString InText, float Time)
{
	if (!InkLineWidget){UE_LOG(LogTemp, Error, TEXT("InkLineWidget is nullptr (HandWidget.cpp)")); return;}
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BattleMainScreen->InkLineTimerHandle);

		World->GetTimerManager().SetTimer(
			BattleMainScreen->InkLineTimerHandle,
			this,
			&UHandWidget::DisplayInkLinebActive,
			Time,
			false
		);
	}
	
	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText( FText::FromString(InText));
	InkLineWidget->PlayInkLine();
}



void UHandWidget::DisplayInkLinebActive()
{
	UE_LOG(LogTemp, Error, TEXT("DisplayInkLinebActiveTEst (HandWidget.cpp)"));
	InkLineWidget->SetVisibility(ESlateVisibility::Hidden);
	BattleMainScreen->HandlePipelineUIFinish();
}

void UHandWidget::DisplayInkLineEnabled(FString InText, float Time)
{
	if (!InkLineWidget){UE_LOG(LogTemp, Error, TEXT("InkLineWidget is nullptr (HandWidget.cpp)")); return;}
	
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(BattleMainScreen->InkLineTimerHandle);

		World->GetTimerManager().SetTimer(
			BattleMainScreen->InkLineTimerHandle,
			this,
			&UHandWidget::DisplayInkLineDisabled,
			Time,
			false
		);
	}
	UE_LOG(LogTemp, Error, TEXT("DisplayInkLine Enabled %s"), *InText);
	InkLineWidget->SetVisibility(ESlateVisibility::Visible);
	InkLineWidget->SetInkText( FText::FromString(InText));
	InkLineWidget->PlayInkLine();
}

void UHandWidget::DisplayInkLineDisabled()
{
	InkLineWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UHandWidget::BuildHandFromCharacter(TArray<UMuksiBattleCardDataAsset*> BattleCardAssets)
{
	if (BattleCardAssets.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildHandFromCharacterData failed: CharacterData is null"));
		return;
	}

	ClearHandCards();

	for (UMuksiBattleCardDataAsset* CardData : BattleCardAssets)
	{
		if (!CardData)
		{
			continue;
		}

		AddCardToHand(CardData);
	}

	OrganizeCards(DefaultCardSpacing);

	/*UE_LOG(
		LogTemp,
		Log,
		TEXT("BuildHandFromCharacterData: %s / CardCount=%d"),
		*GetNameSafe(CharacterData),
		BattleCards.Num()
	);	*/
	
	
}

void UHandWidget::DrawCards(ABattleCharacterBase* BattleCharacter)
{
	if (BattleCharacter->GetAllBattleDeck().IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("DrawCards failed: BattleCardAssets is empty")
		);
		return;
	}
	if (!HandCanvas)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("DrawCards failed: HandCanvas is null")
		);
		return;
	}
	if (!CardDrawSpawnPoint)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("DrawCards failed: CardDrawSpawnPoint is null")
		);
		return;
	}
	BattleCharacter->InitBattleDeck();
	ClearHandCards();
	// SpawnPoint의 CachedGeometry를 사용하기 위해 레이아웃 갱신
	ForceLayoutPrepass();
	
	for (UMuksiBattleCardDataAsset* CardData : BattleCharacter->GetAllBattleDeck())
	{
		if (!CardData)
		{
			continue;
		}

		CreateCardAtDrawSpawnPoint(CardData);
	}
	//화면에 보이게 부채꼴 핸드 위치 설정
	HandCardPoint = CardUpPoint;
	// 오른쪽 생성 지점에서 부채꼴 핸드 위치로 이동
	OrganizeCards(DefaultCardSpacing);
}

UWidget_BattleCardBase* UHandWidget::AddCardToHand(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: CardData is null"));
		return nullptr;
	}

	if (!BattleCardClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: BattleCardClass is null"));
		return nullptr;
	}

	if (!HandCanvas)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: HandCanvas is null"));
		return nullptr;
	}

	UWidget_BattleCardBase* CardWidget =
		CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);

	if (!CardWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: CreateWidget failed"));
		return nullptr;
	}
	CardWidget->SetCardData(CardData);
	PlaceCardInHand(CardWidget);
	
	UE_LOG(
			LogTemp,
			Log,
			TEXT("AddCardToHand: %s"),
			*GetNameSafe(CardData)
		);

	return CardWidget;
}

void UHandWidget::PlaceCardInHand(UWidget_BattleCardBase* InCardWidget)
{
	if (!InCardWidget)
	{
		return;
	}

	if (!HandCanvas)
	{
		return;
	}
	
	if (BattleCards.Contains(InCardWidget))
	{
		return;
	}

	InCardWidget->RemoveFromParent();
	
	InCardWidget->SetOwningHandWidget(this);
	

	if (UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(InCardWidget))
	{
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetZOrder(BattleCards.Num());
	}

	FWidgetCard WidgetCard;
	WidgetCard.Cards = InCardWidget;
	WidgetCard.ZIndex = BattleCards.Num();

	CardsStructArray.Add(WidgetCard);
	BattleCards.Add(InCardWidget);
}




UWidget_BattleCardBase* UHandWidget::CreateCardAtDrawSpawnPoint(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData)
	{
		return nullptr;
	}

	if (!BattleCardClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CreateCardAtDrawSpawnPoint failed: BattleCardClass is null")
		);
		return nullptr;
	}

	if (!HandCanvas || !CardDrawSpawnPoint)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CreateCardAtDrawSpawnPoint failed: required widget is null")
		);
		return nullptr;
	}

	UWidget_BattleCardBase* CardWidget =
		CreateWidget<UWidget_BattleCardBase>(
			GetOwningPlayer(),
			BattleCardClass
		);

	if (!CardWidget)
	{
		return nullptr;
	}

	CardWidget->SetCardData(CardData);
	CardWidget->SetOwningHandWidget(this);

	UCanvasPanelSlot* CanvasSlot =
		HandCanvas->AddChildToCanvas(CardWidget);

	if (!CanvasSlot)
	{
		CardWidget->RemoveFromParent();
		return nullptr;
	}

	CanvasSlot->SetAutoSize(true);

	// HandCanvas 아래 중앙 을 좌표 기준으로 사용
	CanvasSlot->SetAnchors(
	FAnchors(0.5f, 1.0f, 0.5f, 1.0f)
);

	CanvasSlot->SetAlignment(
		FVector2D(0.5f, 1.0f)
	);

	const FGeometry& SpawnGeometry =
		CardDrawSpawnPoint->GetCachedGeometry();

	const FGeometry& HandGeometry =
		HandCanvas->GetCachedGeometry();

	// SpawnPoint 중앙의 절대 좌표
	const FVector2D SpawnAbsolutePosition =
		SpawnGeometry.LocalToAbsolute(
			SpawnGeometry.GetLocalSize() * 0.5f
		);

	// 절대 좌표를 HandCanvas 로컬 좌표로 변환
	const FVector2D SpawnLocalPosition =
		HandGeometry.AbsoluteToLocal(
			SpawnAbsolutePosition
		);
	
	const FVector2D HandBottomCenter(
	HandGeometry.GetLocalSize().X * 0.5f,
	HandGeometry.GetLocalSize().Y
);

	const FVector2D SpawnAnchorOffset =
		SpawnLocalPosition - HandBottomCenter;

	
	////////////////////
	
	CanvasSlot->SetPosition(SpawnAnchorOffset);
	//CanvasSlot->SetPosition(SpawnLocalPosition);
	CanvasSlot->SetZOrder(BattleCards.Num());

	FWidgetCard WidgetCard;
	WidgetCard.Cards = CardWidget;
	WidgetCard.ZIndex = BattleCards.Num();

	CardsStructArray.Add(WidgetCard);
	BattleCards.Add(CardWidget);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("Card created at SpawnPoint | Card: %s | Position: X=%f Y=%f"),
		*GetNameSafe(CardData),
		SpawnLocalPosition.X,
		SpawnLocalPosition.Y
	);

	UE_LOG(
	LogTemp,
	Log,
	TEXT(
		"Card created at SpawnPoint | Card: %s | "
		"SpawnLocal: X=%f Y=%f | "
		"AnchorOffset: X=%f Y=%f"
	),
	*GetNameSafe(CardData),
	SpawnLocalPosition.X,
	SpawnLocalPosition.Y,
	SpawnAnchorOffset.X,
	SpawnAnchorOffset.Y
);
	
	return CardWidget;
}

FVector2D UHandWidget::GetCardDrawStartLocalPosition() const
{
	if (!CardDrawSpawnPoint || !HandCanvas)
	{
		return FVector2D::ZeroVector;
	}

	const FGeometry& SpawnGeometry =
		CardDrawSpawnPoint->GetCachedGeometry();

	const FGeometry& HandGeometry =
		HandCanvas->GetCachedGeometry();

	// SpawnPoint의 중앙 화면 좌표
	const FVector2D SpawnAbsolutePosition =
		SpawnGeometry.LocalToAbsolute(
			SpawnGeometry.GetLocalSize() * 0.5f
		);

	// 화면 좌표를 HandCanvas 기준 로컬 좌표로 변환
	return HandGeometry.AbsoluteToLocal(
		SpawnAbsolutePosition
	);
}


void UHandWidget::RemoveSelectedCardsData()
{
	for (UWidget_BattleCardBase* Widget : RemoveCardArray)
	{
		Widget->RemoveFromParent();
	}
	RemoveCardArray.Empty();
}

void UHandWidget::SetCharacterData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy)
{
	CharacterStatusWidget_Player->SetData(Player);
	CharacterStatusWidget_Enemy->SetData(Enemy);
}


