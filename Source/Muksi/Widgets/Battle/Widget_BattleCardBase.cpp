// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_BattleCardBase.h"

#include "CommonTextBlock.h"
#include "Widget_CardEquipSlot.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Hand/HandWidget.h"
#include "NiagaraSystemWidget.h"
#include "Components/Overlay.h"
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

void UWidget_BattleCardBase::OnAnimationFinished_Implementation(const UWidgetAnimation* Animation)
{
	Super::OnAnimationFinished_Implementation(Animation);

	if (Animation == Anim_CardFlipToFront)
	{
		OnCardFlipFinished.Broadcast(this);
	}
}

void UWidget_BattleCardBase::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	MoveTimeline.TickTimeline(InDeltaTime);

	// 드로우 애니메이션 중에는 드래그하지 않음
	if (bIsDragging &&
		OwningHandWidget &&
		!bPlayingDrawAnimation)
	{
		const FVector2D MouseScreenPos = FSlateApplication::Get().GetCursorPos();

		const FGeometry& HandGeometry = OwningHandWidget->GetHandCanvasGeometry();

		const FVector2D LocalMousePos = HandGeometry.AbsoluteToLocal(MouseScreenPos);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
		{
			CanvasSlot->SetPosition(LocalMousePos - DragOffset);
		}
	}

	if (!bPlayingDrawAnimation)
	{
		return;
	}

	DrawAnimationElapsed += InDeltaTime;

	// 카드별 시작 지연
	if (DrawAnimationElapsed < DrawAnimationDelay)
	{
		return;
	}

	const float AnimationElapsed =
		DrawAnimationElapsed - DrawAnimationDelay;

	const float SafeDuration =
		FMath::Max(
			DrawAnimationDuration,
			KINDA_SMALL_NUMBER
		);

	const float Alpha =
		FMath::Clamp(
			AnimationElapsed / SafeDuration,
			0.0f,
			1.0f
		);

	const float EasedAlpha =
		FMath::InterpEaseOut(
			0.0f,
			1.0f,
			Alpha,
			3.0f
		);

	const FVector2D CurrentTranslation =
		FMath::Lerp(
			DrawStartTranslation,
			FVector2D::ZeroVector,
			EasedAlpha
		);

	SetRenderTranslation(CurrentTranslation);

	const float OpacityAlpha =
		FMath::Clamp(
			Alpha * 4.0f,
			0.0f,
			1.0f
		);

	SetRenderOpacity(OpacityAlpha);

	if (Alpha >= 1.0f)
	{
		SetRenderTranslation(
			FVector2D::ZeroVector
		);

		SetRenderOpacity(1.0f);
		SetIsEnabled(true);

		bPlayingDrawAnimation = false;
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
	/*SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
	SetRenderTransformAngle(InAngle);*/
	SetRenderTransformPivot(FVector2D(0.5f, 1.0f));
	if (!CardRotationRoot)
	{
		return;
	}

	CardRotationRoot->SetRenderTransformAngle(InAngle);
}

void UWidget_BattleCardBase::SetCardInstance(const FGuid& InInstanceId, UMuksiBattleCardDataAsset* InCardData)
{
	CardInstanceId = InInstanceId;
	SetCardData(InCardData);
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
	
	if (OwningHandWidget)
	{
		UWidget_CardEquipSlot* OverlappedSlot =
			OwningHandWidget->FindOverlappedEquipSlot(this);

		if (OverlappedSlot && OverlappedSlot->CanEquipCard(this))
		{
			// 1. 먼저 게임 데이터에서 카드 Commit
			if (OwningHandWidget->CommitHandCard(this))
			{
				// 2. Commit 성공 후 실제 UI를 Slot에 장착
				if (OverlappedSlot->EquipCard(this))
				{
					OwningHandWidget->RemoveHandCardWidget(this);

					OwningHandWidget->OrganizeCards(
						OwningHandWidget->GetDefaultCardSpacing());

					return;
				}

				// 3. Commit은 성공했는데 UI 장착 실패
				// → 게임 데이터 원상복구
				OwningHandWidget->ReturnCommittedHandCard(this);
			}
		}
	}

	MoveToCanvasPosition(CachedHandPosition);

	if (OwningHandWidget)
	{
		OwningHandWidget->OrganizeCards(OwningHandWidget->GetDefaultCardSpacing());
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

void UWidget_BattleCardBase::PlayCardFlipToBack()
{
	if (!Anim_CardFlipToBack)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("Anim_CardFlipToBack is null | Widget: %s (Widget_BattleCardBase.cpp)" ),
			*GetNameSafe(this)
		);

		return;
	}

	PlayAnimation(Anim_CardFlipToBack);
}

void UWidget_BattleCardBase::PlayCardFlipToFront()
{
	if (!Anim_CardFlipToFront)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("CardFlipAnimation is null | Widget: %s (Widget_BattleCardBase.cpp)"),
			*GetNameSafe(this)
		);

		OnCardFlipFinished.Broadcast(this);
		return;
	}

	//PlayAnimation(Anim_CardFlipToFront); 테스트 용도
}

void UWidget_BattleCardBase::PlayDrawToHandAnimation(float InDelay)
{
	DrawAnimationDelay = FMath::Max(0.0f, InDelay);
	DrawAnimationElapsed = 0.0f;
	bPlayingDrawAnimation = true;

	if (!OwningHandWidget)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("PlayDrawToHandAnimation: OwningHandWidget is null")
		);

		bPlayingDrawAnimation = false;
		return;
	}

	const FVector2D SpawnPosition =
		OwningHandWidget->GetCardDrawStartLocalPosition();

	FVector2D FinalCardPosition =
		FVector2D::ZeroVector;

	if (const UCanvasPanelSlot* CanvasSlot =
		Cast<UCanvasPanelSlot>(Slot))
	{
		FinalCardPosition =
			CanvasSlot->GetPosition();
	}

	// 최종 카드 위치에서 SpawnPoint까지의 차이
	DrawStartTranslation = SpawnPosition - FinalCardPosition;

	SetRenderTranslation(DrawStartTranslation);
	SetRenderOpacity(0.0f);
	SetIsEnabled(false);
}

void UWidget_BattleCardBase::PlayCardChangeEffect(UMuksiBattleCardDataAsset* NewCardData)
{
	if (!IsValid(NewCardData))
	{
		return;
	}

	PendingCardData = NewCardData;
	
	PlayAnimation(CardChangeAnimation);
	
	GetWorld()->GetTimerManager().SetTimer(
		CardChangeTimerHandle,
		this,
		&UWidget_BattleCardBase::PlayNiagaraWidget,
		NiagaraWidgetStartTime,
		false);
}

void UWidget_BattleCardBase::StopCardChangeEffect()
{
	if (!CardChangeNiagaraWidget)
	{
		return;
	}

	CardChangeNiagaraWidget->DeactivateSystem();
}

void UWidget_BattleCardBase::PlayNiagaraWidget()
{
	CardChangeNiagaraWidget->ActivateSystem(true);
	
	GetWorld()->GetTimerManager().SetTimer(
		CardChangeTimerHandle,
		this,
		&UWidget_BattleCardBase::ApplyPendingCardChange,
		CardDataChangeTime,
		false);
}

void UWidget_BattleCardBase::ApplyPendingCardChange()
{
	if (!IsValid(PendingCardData))
	{
		return;
	}

	// 기존 카드 표시 갱신 함수 사용
	SetCardData(PendingCardData);

	PendingCardData = nullptr;
}



