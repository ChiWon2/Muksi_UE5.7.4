// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"

#include "CommonTextBlock.h"
#include "HandWidget.h"
#include "Widget_CardEquipSlot.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"


void UWidget_BattleCardBase::NativeConstruct()
{
	Super::NativeConstruct();
	if (MoveCurve)
	{
		FOnTimelineFloat UpdateDelegate;
		UpdateDelegate.BindUFunction(this, FName("OnMoveTimelineUpdate"));

		FOnTimelineEvent FinishedDelegate;
		FinishedDelegate.BindUFunction(this, FName("OnMoveTimelineFinished"));

		MoveTimeline.AddInterpFloat(MoveCurve, UpdateDelegate);
		MoveTimeline.SetTimelineFinishedFunc(FinishedDelegate);
		MoveTimeline.SetLooping(false);
	}
}

void UWidget_BattleCardBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	MoveTimeline.TickTimeline(InDeltaTime);
	
	/*if (bIsDragging && OwningHandWidget)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			const FVector2D MouseViewportPos =
				UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());

			const FGeometry& HandGeometry = OwningHandWidget->GetHandCanvasGeometry();
			const FVector2D LocalMousePos = HandGeometry.AbsoluteToLocal(MouseViewportPos);

			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
			{
				const FVector2D NewPosition = LocalMousePos - DragOffset;
				CanvasSlot->SetPosition(NewPosition);
			}
		}
	}*/
	
	if (bIsDragging && OwningHandWidget)
	{
		const FVector2D MouseScreenPos = FSlateApplication::Get().GetCursorPos();

		const FGeometry& HandGeometry = OwningHandWidget->GetHandCanvasGeometry();
		const FVector2D LocalMousePos = HandGeometry.AbsoluteToLocal(MouseScreenPos);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
		{
			CanvasSlot->SetPosition(LocalMousePos - DragOffset);
		}
	}
}

void UWidget_BattleCardBase::NativeDestruct()
{
	bIsDragging = false;
	MoveTimeline.Stop();
	OwningHandWidget = nullptr;
	
	Super::NativeDestruct();
}

void UWidget_BattleCardBase::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (OwningHandWidget)
	{
		OwningHandWidget->SetHoveredCard(this);
	}
	
}

void UWidget_BattleCardBase::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (OwningHandWidget)
	{
		OwningHandWidget->ClearHoveredCard(this);
	}
}

FReply UWidget_BattleCardBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		StartDragging(InMouseEvent);
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FReply UWidget_BattleCardBase::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
	if (bIsDragging && InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		StopDragging();
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

void UWidget_BattleCardBase::MoveToCanvasPosition(const FVector2D& InTargetPosition)
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		MoveStartPosition = CanvasSlot->GetPosition();
		MoveTargetPosition = InTargetPosition;
		MoveTimeline.PlayFromStart();
	}
}

void UWidget_BattleCardBase::SetOwningHandWidget(UHandWidget* InHandWidget)
{
	OwningHandWidget = InHandWidget;
}

void UWidget_BattleCardBase::SetCardRenderAngle(float InAngle)
{
	SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
	SetRenderTransformAngle(InAngle);
}

void UWidget_BattleCardBase::OnMoveTimelineUpdate(float Alpha)
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		const FVector2D NewPosition = FMath::Lerp(MoveStartPosition, MoveTargetPosition, Alpha);
		CanvasSlot->SetPosition(NewPosition);
	}
}

void UWidget_BattleCardBase::OnMoveTimelineFinished()
{
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CanvasSlot->SetPosition(MoveTargetPosition);
	}
}

void UWidget_BattleCardBase::StartDragging(const FPointerEvent& InMouseEvent)
{
	bIsDragging = true;
	MoveTimeline.Stop();

	if (!OwningHandWidget)
	{
		return;
	}
	
	// 드래그 중에는 카드 똑바로 보이게
	SetRenderTransformAngle(0.0f);
	
	if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
	{
		CachedHandPosition = CanvasSlot->GetPosition();
		CanvasSlot->SetZOrder(9999);

		const FGeometry& HandGeometry = OwningHandWidget->GetHandCanvasGeometry();
		const FVector2D LocalMousePos =
			HandGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

		DragOffset = LocalMousePos - CanvasSlot->GetPosition();
	}
}

void UWidget_BattleCardBase::StopDragging()
{
	bIsDragging = false;
	
	/*if (OwningHandWidget)
	{
		UWidget_CardEquipSlot* OverlappedSlot =
		OwningHandWidget->FindOverlappedEquipSlot(this);
		UE_LOG(LogTemp, Log, TEXT("FindOverlappedEquipLSlot check Test1"));
		if (OverlappedSlot)
		{
			UE_LOG(LogTemp, Log, TEXT("FindOverlappedEquipLSlot check Test2"));
			if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
			{
				UE_LOG(LogTemp, Log, TEXT("FindOverlappedEquipLSlot check Test3"));
				const FVector2D SlotCenter = OverlappedSlot->GetSlotCenterInHandCanvas();
				const FVector2D CardSize = GetCachedGeometry().GetLocalSize();

				const FVector2D SnapPos = SlotCenter - (CardSize * 0.5f);
				MoveToCanvasPosition(SnapPos);

				OverlappedSlot->EquipCard(this);
			}

			OwningHandWidget->RemoveBattleCards(this);
			if (OwningHandWidget)
			{
				OwningHandWidget->OrganizeCards(OwningHandWidget->GetDefaultCardSpacing());
			}
			return;
		}
	}
	

	// 장착 실패면 원위치 복귀
	MoveToCanvasPosition(CachedHandPosition);

	if (OwningHandWidget)
	{
		OwningHandWidget->OrganizeCards(OwningHandWidget->GetDefaultCardSpacing());
	}*/
	
	/*if (OwningHandWidget)
	{
		UWidget_CardEquipSlot* OverlappedSlot =
			OwningHandWidget->FindOverlappedEquipSlot(this);

		if (OverlappedSlot)
		{
			const FVector2D SlotCenter =
				OverlappedSlot->GetSlotCenterInHandCanvas(OwningHandWidget);

			const FVector2D CardSize =
				GetCachedGeometry().GetLocalSize();

			const FVector2D SnapPos =
				SlotCenter - CardSize * 0.5f;

			if (OverlappedSlot->EquipCard(this))
			{
				MoveToCanvasPosition(SnapPos);

				OwningHandWidget->RemoveBattleCards(this);
				OwningHandWidget->OrganizeCards(
					OwningHandWidget->GetDefaultCardSpacing()
				);

				return;
			}
		}
	}

	MoveToCanvasPosition(CachedHandPosition);

	if (OwningHandWidget)
	{
		OwningHandWidget->OrganizeCards(
			OwningHandWidget->GetDefaultCardSpacing()
		);
	}*/
	bIsDragging = false;

	if (OwningHandWidget)
	{
		UWidget_CardEquipSlot* OverlappedSlot =
			OwningHandWidget->FindOverlappedEquipSlot(this);
		if (OverlappedSlot && OverlappedSlot->EquipCard(this))
		{
			OwningHandWidget->RemoveBattleCards(this);
			OwningHandWidget->OrganizeCards(
				OwningHandWidget->GetDefaultCardSpacing()
			);
			return;
		}
	}

	MoveToCanvasPosition(CachedHandPosition);

	if (OwningHandWidget)
	{
		OwningHandWidget->OrganizeCards(
			OwningHandWidget->GetDefaultCardSpacing()
		);
	}
}

void UWidget_BattleCardBase::SetCardData(UMuksiBattleCardDataAsset* InCardData)
{
	CardData = InCardData;
	
	if (Text_CardName)
	{
		Text_CardName->SetText(CardData->CardName);
	}

}

void UWidget_BattleCardBase::SetCardData_(UMuksiBattleCardDataAsset* InCardData, bool bFront)
{
	CardData = InCardData;
	
	if (Text_CardName)
	{
		Text_CardName->SetText(CardData->CardName);
	}
	
	bCardFront = bFront;
	if (bCardFront)
	{
		Border_CardFront->SetRenderScale(FVector2D(1.0f, 1.0f));
		Image_BehindCardImage->SetRenderScale(FVector2D(0.0f, 1.0f));
	}else
	{
		Border_CardFront->SetRenderScale(FVector2D(0.0f, 1.0f));
		Image_BehindCardImage->SetRenderScale(FVector2D(1.0f, 1.0f));
	}
}



