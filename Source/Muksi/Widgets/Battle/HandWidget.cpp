// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/HandWidget.h"


#include "Widget_CardEquipSlot.h"
#include "Components/Button.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Widgets/Components/InkLineWidget.h"
#include "TimerManager.h"

#include "MuksiDebugHelper.h"

FWidgetCard::FWidgetCard()
{
}

void UHandWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	BattleManager = Cast<ABattleManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ABattleManager::StaticClass())
	);
	BattleManager->SetHandWidget(this);
	BattleManager->SettingBattleManager();
	
	Debug::Print(TEXT("Battle Manager Settings"));
	
	if (Button_TurnEnd)
	{
		Button_TurnEnd->OnClicked.AddDynamic(this, &UHandWidget::OnClickedTurnEndButton);
	}

	SpawnDefaultHandCards();
	
	
}

void UHandWidget::NativeDestruct()
{
	if (Button_TurnEnd)
	{
		Button_TurnEnd->OnClicked.RemoveDynamic(this, &UHandWidget::OnClickedTurnEndButton);
	}

	if (BattleManager)
	{
		BattleManager->SetHandWidget(nullptr);
	}

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

void UHandWidget::RemoveBattleCards(UWidget_BattleCardBase* InCard)
{
	BattleCards.Remove(InCard);
}

void UHandWidget::OnClickedTurnEndButton()
{
	Debug::Print(TEXT("Clicked Turn end button"));
	if (!EquipSlotTest)
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
	}
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


