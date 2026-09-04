// Fill out your copyright notice in the Description page of Project Settings.


#include "HandWidget.h"


#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "TimerManager.h"
#include "Card/BattleCardManager.h"


#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

#include "ExchangeSlot/ExchangeSlotPanelWidget.h"
#include "ExchangeSlot/ExchangeSlotTypes.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Widgets/Battle/Widget_CardEquipSlot.h"


void UHandWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (ExchangeSlotPanelWidget)
	{
		ExchangeSlotPanelWidget->OnCardReturnRequested.RemoveAll(this);
		ExchangeSlotPanelWidget->OnCardReturnRequested.AddUObject(this, &UHandWidget::HandleCardReturnRequested);
	}
}

void UHandWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Keep trying while the Slate hierarchy is being reused between rounds.
	// CachedGeometry can stay invalid until a real paint pass, so a fixed retry count is not sufficient.
	if (bOrganizeCardsPending)
	{
		TryOrganizeCards(PendingCardSpacing > 0.0f ? PendingCardSpacing : DefaultCardSpacing);
	}

}

void UHandWidget::NativeDestruct()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(OrganizeCardsTimerHandle);
	}
	if (ExchangeSlotPanelWidget){ExchangeSlotPanelWidget->OnCardReturnRequested.RemoveAll(this);}
	if (BattleCardManager){BattleCardManager->OnBattleHandCardChanged.RemoveDynamic(this,&UHandWidget::HandleBattleHandCardChanged);}
	BattleCardManager = nullptr;


	ClearHandCards();
	Super::NativeDestruct();
}


void UHandWidget::RequestOrganizeCards(float OffsetX)
{
	PendingCardSpacing = OffsetX > 0.0f ? OffsetX : DefaultCardSpacing;
	bOrganizeCardsPending = true;
	OrganizeRetryCount = 0;
	OrganizeNotBeforeFrame = GFrameCounter + 2;

	InvalidateLayoutAndVolatility();
	if (HandCanvas)
	{
		HandCanvas->InvalidateLayoutAndVolatility();
	}
	if (HandCardPoint)
	{
		HandCardPoint->InvalidateLayoutAndVolatility();
	}

	ScheduleOrganizeCards();
}

void UHandWidget::ScheduleOrganizeCards()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(OrganizeCardsTimerHandle);
		OrganizeCardsTimerHandle = World->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UHandWidget::HandleOrganizeCardsTimer));
	}
}

void UHandWidget::HandleOrganizeCardsTimer()
{
	if (!bOrganizeCardsPending)
	{
		return;
	}

	if (TryOrganizeCards(PendingCardSpacing > 0.0f ? PendingCardSpacing : DefaultCardSpacing))
	{
		return;
	}

	++OrganizeRetryCount;
	ScheduleOrganizeCards();
}

bool UHandWidget::TryOrganizeCards(float OffsetX)
{
	if (GFrameCounter < OrganizeNotBeforeFrame)
	{
		return false;
	}

	if (BattleCards.IsEmpty())
	{
		bOrganizeCardsPending = false;
		return true;
	}

	if (!HandCanvas || !HandCardPoint)
	{
		return false;
	}

	ForceLayoutPrepass();
	const FGeometry& HandGeometry = HandCanvas->GetCachedGeometry();
	const FGeometry& PointGeometry = HandCardPoint->GetCachedGeometry();
	if (HandGeometry.GetLocalSize().X <= 1.0f || HandGeometry.GetLocalSize().Y <= 1.0f || PointGeometry.GetLocalSize().X <= 1.0f || PointGeometry.GetLocalSize().Y <= 1.0f)
	{
		return false;
	}

	if (bPlaceCardsAtDrawSpawnPending && CardDrawSpawnPoint)
	{
		const FGeometry& SpawnGeometry = CardDrawSpawnPoint->GetCachedGeometry();
		if (SpawnGeometry.GetLocalSize().X <= 1.0f || SpawnGeometry.GetLocalSize().Y <= 1.0f)
		{
			return false;
		}
		const FVector2D SpawnAbsolute = SpawnGeometry.LocalToAbsolute(SpawnGeometry.GetLocalSize() * 0.5f);
		const FVector2D SpawnLocal = HandGeometry.AbsoluteToLocal(SpawnAbsolute);
		const FVector2D HandBottomCenter(HandGeometry.GetLocalSize().X * 0.5f, HandGeometry.GetLocalSize().Y);
		const FVector2D SpawnOffset = SpawnLocal - HandBottomCenter;
		for (UWidget_BattleCardBase* Card : BattleCards)
		{
			if (UCanvasPanelSlot* CardCanvasSlot = Card ? Cast<UCanvasPanelSlot>(Card->Slot) : nullptr)
			{
				CardCanvasSlot->SetPosition(SpawnOffset);
			}
		}
		bPlaceCardsAtDrawSpawnPending = false;
	}

	bOrganizeCardsPending = false;
	OrganizeCards(OffsetX);
	return true;
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
		UE_LOG(LogTemp, Warning, TEXT("OrganizeCards failed: HandCanvas or HandCardPoint is null"));
		return;
	}
	/*
	 * HandCardPoint의 중앙 좌표를 HandCanvas 로컬 좌표로 변환
	 */
	const FGeometry& HandGeometry = HandCanvas->GetCachedGeometry();

	const FGeometry& HandCardPointGeometry = HandCardPoint->GetCachedGeometry();

	const FVector2D HandCardPointAbsoluteCenter = HandCardPointGeometry.LocalToAbsolute(HandCardPointGeometry.GetLocalSize() * 0.5f);

	const FVector2D HandCardPointLocalCenter = HandGeometry.AbsoluteToLocal(HandCardPointAbsoluteCenter);

	/*
	 * 카드의 CanvasSlot Anchor가 (0.5, 1.0)이므로
	 * HandCanvas 아래쪽 중앙이 Position (0, 0)의 기준이다.
	 */
	const FVector2D HandBottomCenter(HandGeometry.GetLocalSize().X * 0.5f, HandGeometry.GetLocalSize().Y);

	/*
	 * HandCanvas 아래쪽 중앙에서 HandCardPoint 오프셋
	 */
	const FVector2D FanCenterOffset = HandCardPointLocalCenter - HandBottomCenter;

	const int32 CardCount = BattleCards.Num();
	const float CenterIndex = (CardCount - 1) * 0.5f;

	const float CurveHeight = 12.0f;
	const float AnglePerCard = 7.0f;
	const float HoverSpreadOffset = 25.0f;

	int32 HoveredIndex = INDEX_NONE;

	if (HoveredCard)
	{
		HoveredIndex = BattleCards.IndexOfByKey(HoveredCard);
	}

	for (int32 i = 0; i < CardCount; ++i)
	{
		UWidget_BattleCardBase* CardWidget = BattleCards[i];

		if (!CardWidget)
		{
			continue;
		}

		const float RelativeIndex = i - CenterIndex;

		const bool bIsHovered = (CardWidget == HoveredCard);

		float AdditionalXOffset = 0.0f;

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

		/*
		 * HandCardPoint 중심으로 한 부채꼴 상대 좌표
		 */
		const float BaseX = RelativeIndex * OffsetX;

		const float BaseY = FMath::Square(RelativeIndex) * CurveHeight;

		const float HoverOffsetY = bIsHovered ? -25.0f : 0.0f;

		/*
		 * 기존 부채꼴 위치에 HandCardPoint 중심 오프셋 추가
		 */
		const float TargetX = FanCenterOffset.X + BaseX + AdditionalXOffset;

		const float TargetY = FanCenterOffset.Y + BaseY + HoverOffsetY;

		float TargetAngle = RelativeIndex * AnglePerCard;

		if (bIsHovered)
		{
			TargetAngle *= 0.35f;
		}

		CardWidget->MoveToCanvasPosition(FVector2D(TargetX, TargetY));

		CardWidget->SetCardRenderAngle(TargetAngle);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(CardWidget->Slot))
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
	bOrganizeCardsPending = false;
	OrganizeRetryCount = 0;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(OrganizeCardsTimerHandle);
	}
}

void UHandWidget::InvisibleHandCards()
{
	if (BattleCards.Num() == 0)
	{
		return;
	}
	else
	{
		HandCardPoint = CardDownPoint;
		RequestOrganizeCards(DefaultCardSpacing);
	}
}

void UHandWidget::VisibleHandCards()
{
	HandCardPoint = CardUpPoint;
	RequestOrganizeCards(DefaultCardSpacing);
}

void UHandWidget::HitActiveHandCards(bool bHitActive)
{
	if (bHitActive)
	{
		for (UWidget_BattleCardBase* CardWidget : BattleCards)
		{
			CardWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
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
	RequestOrganizeCards(DefaultCardSpacing);
}

void UHandWidget::ClearHoveredCard(UWidget_BattleCardBase* InCard)
{
	if (HoveredCard == InCard)
	{
		HoveredCard = nullptr;
		RequestOrganizeCards(DefaultCardSpacing);
	}
}

const FGeometry& UHandWidget::GetHandCanvasGeometry() const
{
	return HandCanvas->GetCachedGeometry();
}

void UHandWidget::RemoveHandCardWidget(UWidget_BattleCardBase* InCard)
{
	if (!InCard)
	{
		return;
	}

	BattleCards.Remove(InCard);

	if (HoveredCard == InCard)
	{
		HoveredCard = nullptr;
	}
}




UWidget_CardEquipSlot* UHandWidget::FindOverlappedEquipSlot(UWidget_BattleCardBase* Card) const
{
	if (!ExchangeSlotPanelWidget)
	{
		return nullptr;
	}

	return ExchangeSlotPanelWidget->FindOverlappedEquipSlot(Card);
}

void UHandWidget::EnableExchangeSlot(int32 Index, bool bActive)
{
	if (ExchangeSlotPanelWidget)
	{
		ExchangeSlotPanelWidget->EnableExchangeSlot(Index, bActive);
	}
}

bool UHandWidget::ApplyPlayerPanicTimeoutResult(int32 ExchangeIndex, const FCharacterPanicTimeoutResult& Result)
{
	if (!ExchangeSlotPanelWidget || !Result.PanicCard.IsValid())
    {
        return false;
    }
	
    UWidget_CardEquipSlot* CurrentSlot = ExchangeSlotPanelWidget->GetPlayerSlotByExchangeNumber(ExchangeIndex + 1);

    if (!CurrentSlot)
    {
        return false;
    }


	//패닉 카드가 있으면 넘어가기
    if (!Result.bReturnedCard)
    {
        UWidget_BattleCardBase* ExistingWidget = CurrentSlot->GetEquipSlot();

        if (ExistingWidget && ExistingWidget->GetCardInstanceId() == Result.PanicCard.InstanceId)
        {
            CurrentSlot->ConfirmSlot();
            return true;
        }
    }
	
	//1. 슬롯에 카드 위젯이 있을 경우 
    if (Result.bReturnedCard)
    {
        UWidget_BattleCardBase* SlotCardWidget = CurrentSlot->GetEquipSlot();

        if (!SlotCardWidget || SlotCardWidget->GetCardInstanceId() != Result.ReturnedCard.InstanceId)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[HandWidget] Returned slot card "
                    "widget does not match result"));

            return false;
        }

        // 카드Id 이미 반환되었으니 UI만 슬롯에서 분리
        SlotCardWidget = CurrentSlot->ReleaseCard();

        if (!SlotCardWidget)
        {
            return false;
        }

        const bool bReturnedCardWasDiscarded = Result.bDiscardedCard && Result.DiscardedCard.InstanceId == Result.ReturnedCard.InstanceId;

        if (bReturnedCardWasDiscarded)
        {
            // 반환된 카드가 랜덤 제거 카드로 다시 선택된 경우
            SlotCardWidget->RemoveFromParent();
        }
        else
        {
            // 반환됐지만 버려지지는 않았으므로 실제 위젯을 손패로 이동
            SlotCardWidget->SetCardRenderAngle(0.0f);
            SlotCardWidget->SetVisibility(
                ESlateVisibility::Visible);

            PlaceCardInHand(SlotCardWidget);
        }
    }

    //2. 핸드에 있는 카드 한장 버리기
    if (Result.bDiscardedCard)
    {
        const bool bDiscardedCardWasReturnedCard =
            Result.bReturnedCard && Result.DiscardedCard.InstanceId == Result.ReturnedCard.InstanceId;

        if (!bDiscardedCardWasReturnedCard)
        {
            UWidget_BattleCardBase* DiscardedWidget = FindBattleCardWidgetByInstanceId(Result.DiscardedCard.InstanceId);

            if (DiscardedWidget)
            {
                RemoveHandCardWidget(DiscardedWidget);
                DiscardedWidget->RemoveFromParent();
            }
            else
            {
                UE_LOG(
                    LogTemp,
                    Warning,
                    TEXT(
                        "[HandWidget] Discarded card widget "
                        "not found: %s"),
                    *GetNameSafe(
                        Result.DiscardedCard.CardData));
            }
        }
    }

    //3. 패닉 카드 생성해 슬롯에 장착
    UWidget_BattleCardBase* PanicCardWidget = CreateCardAtDrawSpawnPoint(Result.PanicCard);

    if (!PanicCardWidget)
    {
        return false;
    }

    if (!CurrentSlot->EquipCard(PanicCardWidget))
    {
        RemoveHandCardWidget(PanicCardWidget);
        PanicCardWidget->RemoveFromParent();

        UE_LOG(
            LogTemp,
            Error,
            TEXT("[HandWidget] Failed to equip panic card widget"));

        return false;
    }

    // CreateCardAtDrawSpawnPoint가 BattleCards에 추가했으므로
    // 슬롯 장착 후 손패 배열에서는 제거한다.
    RemoveHandCardWidget(PanicCardWidget);

    // 패닉 카드는 사용자가 다시 꺼낼 수 없게 한다.
    CurrentSlot->ConfirmSlot();

    OrganizeCards(GetDefaultCardSpacing());

    return true;
}

void UHandWidget::NotifyPlayerCardEquipped()
{
	OnPlayerCardEquipped.Broadcast();
}

void UHandWidget::HandleCardReturnRequested(UWidget_BattleCardBase* CardWidget)
{
	if (!CardWidget)
	{
		return;
	}

	if (!ReturnCommittedHandCard(CardWidget))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("HandleCardReturnRequested - ReturnCommittedHandCard Failed: %s"),
			*GetNameSafe(CardWidget->GetCardData()));

		return;
	}
	
	CardWidget->SetCardRenderAngle(0.0f);
	CardWidget->SetVisibility(ESlateVisibility::Visible);

	PlaceCardInHand(CardWidget);

	OrganizeCards(GetDefaultCardSpacing());
	OnPlayerCardReturned.Broadcast();
}


void UHandWidget::BindingBattleCardManager(UBattleCardManager* InBattleCardManager)
{
	if (BattleCardManager)
	{
		BattleCardManager->OnBattleHandCardChanged.RemoveDynamic(
			this,
			&UHandWidget::HandleBattleHandCardChanged);
	}

	BattleCardManager = InBattleCardManager;

	if (BattleCardManager)
	{
		BattleCardManager->OnBattleHandCardChanged.AddUniqueDynamic(
			this,
			&UHandWidget::HandleBattleHandCardChanged);
	}
}

void UHandWidget::HandleBattleHandCardChanged(FGuid InstanceId,
	UMuksiBattleCardDataAsset* NewCardData)
{
	UWidget_BattleCardBase* CardWidget =
		FindBattleCardWidgetByInstanceId(InstanceId);

	if (!IsValid(CardWidget))
	{
		return;
	}

	CardWidget->PlayCardChangeEffect(NewCardData);
}

UWidget_BattleCardBase* UHandWidget::FindBattleCardWidgetByInstanceId(const FGuid& InstanceId) const
{
	for (UWidget_BattleCardBase* CardWidget : BattleCards)
	{
		if (!IsValid(CardWidget))
		{
			continue;
		}

		if (CardWidget->GetCardInstanceId() == InstanceId)
		{
			return CardWidget;
		}
	}

	return nullptr;
}

void UHandWidget::ClearEnemySelectCard()
{
	
	if (!ExchangeSlotPanelWidget)
	{
		return;
	}
	for (const FReleasedExchangeCard& Released
	: ExchangeSlotPanelWidget->ReleaseEnemyCards())
	{
		MoveCardToRemovePoint(
			Released.CardWidget,
			Released.AbsoluteBottomCenter,
			CardRemovePoint_Enemy
		);
	}
}

void UHandWidget::ClearPlayerSelectCard()
{
	
	
	if (!ExchangeSlotPanelWidget)
	{
		return;
	}
	for (const FReleasedExchangeCard& Released
	: ExchangeSlotPanelWidget->ReleasePlayerCards())
	{
		MoveCardToRemovePoint(
			Released.CardWidget,
			Released.AbsoluteBottomCenter,
			CardRemovePoint_Player
		);
	}
}


void UHandWidget::ClearTimerHandler()
{
	GetWorld()->GetTimerManager().ClearTimer(OrganizeCardsTimerHandle);
}

void UHandWidget::MoveCardToRemovePoint(UWidget_BattleCardBase* CardWidget, const FVector2D& StartAbsolutePosition,
	UWidget* RemovePoint)
{
	if (!CardWidget || !HandCanvas || !RemovePoint)
	{
		return;
	}

	UCanvasPanelSlot* CardCanvasSlot =
		HandCanvas->AddChildToCanvas(CardWidget);

	if (!CardCanvasSlot)
	{
		return;
	}

	CardCanvasSlot->SetAutoSize(true);
	CardCanvasSlot->SetAnchors(
		FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
	CardCanvasSlot->SetAlignment(
		FVector2D(0.5f, 1.0f));

	ForceLayoutPrepass();

	const FGeometry& HandGeometry =
		HandCanvas->GetCachedGeometry();

	const FVector2D HandBottomCenter(
		HandGeometry.GetLocalSize().X * 0.5f,
		HandGeometry.GetLocalSize().Y
	);

	// ExchangeSlot에 있었던 기존 화면 위치
	const FVector2D StartLocalPosition =
		HandGeometry.AbsoluteToLocal(
			StartAbsolutePosition);

	const FVector2D StartAnchorOffset =
		StartLocalPosition - HandBottomCenter;

	CardCanvasSlot->SetPosition(StartAnchorOffset);
	CardCanvasSlot->SetZOrder(10000);

	CardWidget->SetOwningHandWidget(this);
	CardWidget->SetCardRenderAngle(0.0f);

	const FGeometry& RemovePointGeometry =
		RemovePoint->GetCachedGeometry();

	const FVector2D RemoveAbsolutePosition =
		RemovePointGeometry.LocalToAbsolute(
			RemovePointGeometry.GetLocalSize() * 0.5f);

	const FVector2D RemoveLocalPosition =
		HandGeometry.AbsoluteToLocal(
			RemoveAbsolutePosition);

	const FVector2D RemoveAnchorOffset =
		RemoveLocalPosition - HandBottomCenter;

	CardWidget->MoveToCanvasPosition(
		RemoveAnchorOffset);

	RemoveCardArray.Add(CardWidget);
}

void UHandWidget::DrawCards()
{
	if (!BoundCharacter)
	{
		return;
	}
	if (!HandCanvas)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawCards failed: HandCanvas is null"));
		return;
	}
	if (!CardDrawSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("DrawCards failed: CardDrawSpawnPoint is null"));
		return;
	}
	
	UBattleCardComponent* CardComponent = BoundCharacter->GetBattleCardComponent();
	if (!CardComponent)
	{
		return;
	}
	
	ClearHandCards();
	// 레이아웃 갱신 (SpawnPoint의 CachedGeometry를 사용하기 위해)
	ForceLayoutPrepass();
	for (const FBattleCardInstance& CardInstance : CardComponent->GetCurrentHand())
	{
		if (!CardInstance.IsValid())
		{
			continue;
		}

		UWidget_BattleCardBase* CardWidget = CreateCardAtDrawSpawnPoint(CardInstance);

		if (!CardWidget)
		{
			continue;
		}
	}
	
	//화면에 보이게 부채꼴 핸드 위치 설정
	HandCardPoint = CardUpPoint;
	// Place the cards from the current draw point only after all geometries are valid.
	bPlaceCardsAtDrawSpawnPending = true;
	RequestOrganizeCards(DefaultCardSpacing);
}

bool UHandWidget::HasHandCardWidgets() const
{
	return !BattleCards.IsEmpty();
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
	BattleCards.Add(InCardWidget);
}

//1. 카드의 InstanceID 얻기
//2. BoundCharacter의 BattleCardComponent 찾기 
//3. CommitCard()호출
bool UHandWidget::CommitHandCard(UWidget_BattleCardBase* CardWidget)
{
	if (!CardWidget || !BoundCharacter)
	{
		return false;
	}

	UBattleCardComponent* CardComponent = BoundCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	return CardComponent->CommitCard(CardWidget->GetCardInstanceId());
}

bool UHandWidget::ReturnCommittedHandCard(UWidget_BattleCardBase* CardWidget)
{
	if (!CardWidget || !BoundCharacter)
	{
		return false;
	}

	UBattleCardComponent* CardComponent =
		BoundCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	return CardComponent->ReturnCommittedCard(
		CardWidget->GetCardInstanceId());
}


UWidget_BattleCardBase* UHandWidget::CreateCardAtDrawSpawnPoint(const FBattleCardInstance& CardInstance)
{
	if (!CardInstance.CardData)
	{
		return nullptr;
	}
	
	if (!BattleCardClass || !HandCanvas || !CardDrawSpawnPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("CreateCardAtDrawSpawnPoint failed: invalid input or widget reference"));
		return nullptr;
	}

	UWidget_BattleCardBase* CardWidget = CreateWidget<UWidget_BattleCardBase>(GetOwningPlayer(), BattleCardClass);
	if (!CardWidget)
	{
		return nullptr;
	}

	CardWidget->SetCardInstance(CardInstance.InstanceId,CardInstance.CardData);
	CardWidget->SetOwningHandWidget(this);

	UCanvasPanelSlot* CanvasSlot = HandCanvas->AddChildToCanvas(CardWidget);
	if (!CanvasSlot)
	{
		CardWidget->RemoveFromParent();
		return nullptr;
	}

	CanvasSlot->SetAutoSize(true);
	CanvasSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
	CanvasSlot->SetAlignment(FVector2D(0.5f, 1.0f));
	// The real spawn offset is assigned after Slate has produced valid geometry.
	CanvasSlot->SetPosition(FVector2D::ZeroVector);
	CanvasSlot->SetZOrder(BattleCards.Num());
	
	BattleCards.Add(CardWidget);
	return CardWidget;
}

FVector2D UHandWidget::GetCardDrawStartLocalPosition() const
{
	if (!CardDrawSpawnPoint || !HandCanvas)
	{
		return FVector2D::ZeroVector;
	}

	const FGeometry& SpawnGeometry = CardDrawSpawnPoint->GetCachedGeometry();

	const FGeometry& HandGeometry = HandCanvas->GetCachedGeometry();

	// SpawnPoint의 중앙 화면 좌표
	const FVector2D SpawnAbsolutePosition = SpawnGeometry.LocalToAbsolute(SpawnGeometry.GetLocalSize() * 0.5f);

	// 화면 좌표를 HandCanvas 기준 로컬 좌표로 변환
	return HandGeometry.AbsoluteToLocal(SpawnAbsolutePosition);
}


void UHandWidget::RemoveSelectedCardsData()
{
	for (UWidget_BattleCardBase* Widget : RemoveCardArray)
	{
		Widget->RemoveFromParent();
	}
	RemoveCardArray.Empty();
}

void UHandWidget::SetBattleCharacter(ABattleCharacterBase* InBattleCharacter)
{
	BoundCharacter = InBattleCharacter;
}

