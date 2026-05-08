// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/HandWidget.h"


#include "Widget_CardEquipSlot.h"
#include "Components/Button.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Widgets/Components/InkLineWidget.h"
#include "TimerManager.h"


#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "MuksiDebugHelper.h"



FWidgetCard::FWidgetCard()
{
	
}

void UHandWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	
	
	
	Debug::Print(TEXT("Battle Manager Settings"));
	
	BindEndTurnButton();

	//SpawnDefaultHandCards();
	
	InitializeExchangeSlots();
}

void UHandWidget::NativeDestruct()
{
	

	UnbindEndTurnButton();
	
	
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
			/*CanvasSlot->SetAutoSize(true);
			
			CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
			
			CanvasSlot->SetPosition(FVector2D(LiteralFloatX, LiteralFloatY));
			CanvasSlot->SetZOrder(i);*/
			
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
	if (BattleCards.Num() == 0)
	{
		return;
	}

	const int32 CardCount = BattleCards.Num();
	const float CenterIndex = (CardCount - 1) * 0.5f;

	const float CurveHeight = 12.f;
	const float AnglePerCard = 7.f;

	// hover 카드가 있을 때 양옆으로 벌어지는 정도
	const float HoverSpreadOffset = 25.f;

	int32 HoveredIndex = INDEX_NONE;
	if (HoveredCard)
	{
		HoveredIndex = BattleCards.IndexOfByKey(HoveredCard);
	}

	for (int32 i = 0; i < CardCount; ++i)
	{
		UWidget_BattleCardBase* Widget_BattleCard = BattleCards[i];
		if (!Widget_BattleCard)
		{
			continue;
		}

		const float RelativeIndex = i - CenterIndex;
		const bool bIsHovered = (Widget_BattleCard == HoveredCard);

		float AdditionalXOffset = 0.f;

		if (HoveredIndex != INDEX_NONE)
		{
			if (i < HoveredIndex)
			{
				AdditionalXOffset = -HoverSpreadOffset;
			}
			else if (i > HoveredIndex)
			{
				AdditionalXOffset = HoverSpreadOffset;
			}
		}

		const float BaseX = RelativeIndex * OffsetX;
		const float BaseY = FMath::Square(RelativeIndex) * CurveHeight;

		const float HoverOffsetY = bIsHovered ? -25.f : 0.f;

		const float TargetX = BaseX + AdditionalXOffset;
		const float TargetY = BaseY + HoverOffsetY;

		float TargetAngle = RelativeIndex * AnglePerCard;

		// hover한 카드는 각도를 조금 줄여서 더 강조
		if (bIsHovered)
		{
			TargetAngle *= 0.35f;
		}

		Widget_BattleCard->MoveToCanvasPosition(FVector2D(TargetX, TargetY));
		Widget_BattleCard->SetCardRenderAngle(TargetAngle);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget_BattleCard->Slot))
		{
			CanvasSlot->SetZOrder(bIsHovered ? 999 : i);
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
		HandCanvas->ClearChildren();
	}
	HoveredCard = nullptr;
	BattleCards.Empty();
	CardsStructArray.Empty();
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
	Debug::Print(TEXT("Clicked Turn end button"));
	/*if (!EquipSlotTest)
	{
		return;
	}
	const bool bRemoved = EquipSlotTest->ClearEquipSlot();
	if (!bRemoved)
	{
		return;
	}
	if (BattleManager)
	{
		BattleManager->EndExchange();
	}*/
}

void UHandWidget::ActiveInkLine(FText& DisplayText,float DisplayTime)
{
	if (!InkLine)
	{
		UE_LOG(LogTemp, Error, TEXT("InkLine is nullptr"));
		return;
	}

	InkLine->SetVisibility(ESlateVisibility::Visible);
	InkLine->SetInkText(DisplayText);
	InkLine->PlayInkLine();
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&UHandWidget::DeActiveInkLine,
		DisplayTime,
		false
	);
}

void UHandWidget::DeActiveInkLine()
{
	InkLine->ResetInkLine();
	InkLine->SetVisibility(ESlateVisibility::Hidden);
}

void UHandWidget::ShowTurnEndButton(bool bShow)
{
	if (!Button_TurnEnd)
	{
		return;
	}

	Button_TurnEnd->SetVisibility(
		bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed
	);

	Button_TurnEnd->SetIsEnabled(bShow);
}

FCardEquipSlotData UHandWidget::GetSlotDataByExchangeNumber(int32 InIndex)
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

void UHandWidget::ConfirmExchangeInput(int32 InIndex)
{
	UWidget_CardEquipSlot* EquipSlot = GetSlotByExchangeNumber(InIndex);

	if (!EquipSlot)
	{
		return;
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

		EquipSlot->SetSlotInfo(SlotIndex, ExchangeNumber);
		EquipSlot->ClearSlot();
		/*EquipSlot->SetSlotEnabled(false);
		EquipSlot->SetSlotHighlighted(false);
		EquipSlot->SetSlotConfirmed(false);*/
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

void UHandWidget::BindEndTurnButton()
{
	if (!Button_TurnEnd)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandWidget: EndTurnButton is null"));
		return;
	}

	// 중복 바인딩 방지
	Button_TurnEnd->OnClicked.RemoveAll(this);
	//Button_TurnEnd->OnClicked().RemoveAll(this);

	Button_TurnEnd->OnClicked.AddDynamic(
		this,
		&UHandWidget::HandleEndTurnButtonClicked
	);
}

void UHandWidget::UnbindEndTurnButton()
{
	if (!Button_TurnEnd)
	{
		return;
	}

	Button_TurnEnd->OnClicked.RemoveAll(this);
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

void UHandWidget::BuildHandFromCharacterData(TArray<UMuksiBattleCardDataAsset*> BattleCardAssets)
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

void UHandWidget::AddCardToHand(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: CardData is null"));
		return;
	}

	if (!BattleCardClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: BattleCardClass is null"));
		return;
	}

	if (!HandCanvas)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: HandCanvas is null"));
		return;
	}

	UWidget_BattleCardBase* CardWidget =
		CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);

	if (!CardWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("AddCardToHand failed: CreateWidget failed"));
		return;
	}

	CardWidget->SetOwningHandWidget(this);
	CardWidget->SetCardData(CardData);

	if (UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(CardWidget))
	{
		CanvasSlot->SetAutoSize(true);
		CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
		CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
		CanvasSlot->SetPosition(FVector2D::ZeroVector);
		CanvasSlot->SetZOrder(BattleCards.Num());
	}

	FWidgetCard WidgetCard;
	WidgetCard.Cards = CardWidget;
	WidgetCard.ZIndex = BattleCards.Num();

	CardsStructArray.Add(WidgetCard);
	BattleCards.Add(CardWidget);

	UE_LOG(LogTemp, Log, TEXT("AddCardToHand: %s"), *GetNameSafe(CardData));
}

void UHandWidget::BuildHandBattleStart()
{
	
}


