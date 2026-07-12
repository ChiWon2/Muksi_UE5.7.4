// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/HandWidget.h"


#include "CommonButtonBase.h"
#include "Widget_CardEquipSlot.h"
#include "Components/Button.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Widgets/Components/InkLineWidget.h"
#include "CommonButtonBase.h"
#include "TimerManager.h"


#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

#include "MuksiDebugHelper.h"
#include "Components/VerticalBox.h"
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
	
	
	for (UWidget_CardEquipSlot* EquipSlot : EnemyExchangeSlots)
	{
		EquipSlot->ClearSlot();
	}
	EnemySelectedBattleCards.Empty();
}

void UHandWidget::ClearPlayerSelectCard()
{
	UE_LOG(LogTemp, Error, TEXT("1"));
	for (UWidget_CardEquipSlot* EquipSlot : ExchangeSlots)
	{
		UE_LOG(LogTemp, Error, TEXT("2"));
		EquipSlot->ClearSlot();
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
	CardWidget->SetCardData(CardData);
	PlaceCardInHand(CardWidget);

	

	UE_LOG(LogTemp, Log, TEXT("AddCardToHand: %s"), *GetNameSafe(CardData));
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

void UHandWidget::BuildHandBattleStart()
{
	
}


