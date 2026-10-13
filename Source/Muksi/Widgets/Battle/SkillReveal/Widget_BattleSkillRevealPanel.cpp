// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/SkillReveal/Widget_BattleSkillRevealPanel.h"

#include "Widget_BattleSkillRevealSlot.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Framework/Application/SlateApplication.h"

void UWidget_BattleSkillRevealPanel::SetExchangeSkills(int32 ExchangeIndex, UMuksiBattleCardDataAsset* FirstSkill,
                                                       bool bFirstIsPlayer, UMuksiBattleCardDataAsset* SecondSkill, bool bSecondIsPlayer)
{
	if (!FirstSlots.IsValidIndex(ExchangeIndex) ||
	   !SecondSlots.IsValidIndex(ExchangeIndex))
	{
		return;
	}

	if (FirstSlots[ExchangeIndex])
	{
		FirstSlots[ExchangeIndex]->SetSkillData(
			FirstSkill,
			bFirstIsPlayer);
	}

	if (SecondSlots[ExchangeIndex])
	{
		SecondSlots[ExchangeIndex]->SetSkillData(
			SecondSkill,
			bSecondIsPlayer);
	}
}

void UWidget_BattleSkillRevealPanel::ClearAllSkills()
{
	if (IsValid(PendingDeceiveSlot))
	{
		PendingDeceiveSlot->OnDeceiveRevealFinished.RemoveAll(this);
	}

	PendingDeceiveSlot = nullptr;
	
	for (UWidget_BattleSkillRevealSlot* Slot_ : FirstSlots)
	{
		if (Slot_)
		{
			Slot_->ClearSkill();
			Slot_->SetRenderOpacity(0.0f);
		}
	}

	for (UWidget_BattleSkillRevealSlot* Slot_ : SecondSlots)
	{
		if (Slot_)
		{
			Slot_->ClearSkill();
			Slot_->SetRenderOpacity(0.0f);
		}
	}

	PendingRevealExchangeIndex = INDEX_NONE;
	PendingRevealCount = 0;
	
	
	bRevealPresentationRunning = false;
	RevealHoldElapsed = 0.0f;
}

bool UWidget_BattleSkillRevealPanel::RevealExchangeSkills(int32 ExchangeIndex, UMuksiBattleCardDataAsset* FirstSkill,
                                                          bool bFirstIsPlayer, UMuksiBattleCardDataAsset* SecondSkill, bool bSecondIsPlayer)
{
	if (!FirstSlots.IsValidIndex(ExchangeIndex)
	   || !SecondSlots.IsValidIndex(ExchangeIndex))
	{
		return false;
	}

	UWidget_BattleSkillRevealSlot* FirstSlot = FirstSlots[ExchangeIndex];

	UWidget_BattleSkillRevealSlot* SecondSlot = SecondSlots[ExchangeIndex];

	if (!FirstSlot || !SecondSlot)
	{
		return false;
	}

	// 데이터 먼저 세팅
	FirstSlot->SetSkillData(FirstSkill, bFirstIsPlayer);

	SecondSlot->SetSkillData(SecondSkill, bSecondIsPlayer);

	PendingRevealExchangeIndex = ExchangeIndex;
	PendingRevealCount = 2;

	bRevealPresentationRunning = true;
	RevealHoldElapsed = 0.0f;
	
	// 완료 콜백 연결
	FirstSlot->OnRevealFinished.RemoveAll(this);
	SecondSlot->OnRevealFinished.RemoveAll(this);

	FirstSlot->OnRevealFinished.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotRevealFinished);

	SecondSlot->OnRevealFinished.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotRevealFinished);

	// 첫 번째 공개
	if (!FirstSlot->PlayRevealAnimation())
	{
		HandleSlotRevealFinished(FirstSlot);
	}

	// 두 번째 공개
	if (!SecondSlot->PlayRevealAnimation())
	{
		HandleSlotRevealFinished(SecondSlot);
	}

	return true;
}

void UWidget_BattleSkillRevealPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Overlay_Slots)
	{
		BaseSlotsTranslation = Overlay_Slots->GetRenderTransform().Translation;
	}

	bHoverAreaHovered = false;
	bRevealPresentationRunning = false;
	RevealHoldElapsed = 0.0f;
	
	FirstSlots = {
		Slot_Exchange1_First,
		Slot_Exchange2_First,
		Slot_Exchange3_First
	};

	SecondSlots = {
		Slot_Exchange1_Second,
		Slot_Exchange2_Second,
		Slot_Exchange3_Second
	};
	
	for (UWidget_BattleSkillRevealSlot* Slot_ : FirstSlots)
	{
		if (Slot_)
		{
			Slot_->SetRenderOpacity(0.0f);

			Slot_->OnSkillHovered.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotHovered);
			Slot_->OnSkillUnhovered.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotUnhovered);
		}
		
	}

	for (UWidget_BattleSkillRevealSlot* Slot_ : SecondSlots)
	{
		if (Slot_)
		{
			Slot_->SetRenderOpacity(0.0f);
			
			Slot_->OnSkillHovered.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotHovered);
			Slot_->OnSkillUnhovered.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotUnhovered);
		}
	}
}

void UWidget_BattleSkillRevealPanel::HandleSlotRevealFinished(UWidget_BattleSkillRevealSlot* FinishedSlot)
{
	if (!FinishedSlot
		|| PendingRevealExchangeIndex == INDEX_NONE
		|| PendingRevealCount <= 0)
	{
		return;
	}

	FinishedSlot->OnRevealFinished.RemoveAll(this);

	--PendingRevealCount;

	TryFinishRevealPresentation();
}

bool UWidget_BattleSkillRevealPanel::PlayDeceiveReveal(int32 ExchangeIndex, bool bPlayerAction)
{
	if (IsValid(PendingDeceiveSlot))
	{
		return false;
	}
	
	UWidget_BattleSkillRevealSlot* RevealSlot = FindActionSlot(ExchangeIndex, bPlayerAction);
	
	
	if (!RevealSlot)
	{
		return false;
	}
	//RevealSlot->SetSkillData(ActualSkill, bPlayerAction);
	PendingDeceiveSlot = RevealSlot;
	RevealSlot->OnDeceiveRevealFinished.RemoveAll(this);
	RevealSlot->OnDeceiveRevealFinished.AddUObject(this, &UWidget_BattleSkillRevealPanel::HandleSlotDeceiveRevealFinished);
	
	if (!RevealSlot->RevealDeceive())
	{
		RevealSlot->OnDeceiveRevealFinished.RemoveAll(this);
		PendingDeceiveSlot = nullptr;

		return false;
	}

	/*
	// TODO 변초 공개 연출 넣고 거기서 Broadcast 하기
	OnDeceiveRevealFinished.Broadcast();
	*/
	return true;
}

UWidget_BattleSkillRevealSlot* UWidget_BattleSkillRevealPanel::FindActionSlot(int32 ExchangeIndex,
	bool bPlayerAction) const
{
	if (!FirstSlots.IsValidIndex(ExchangeIndex)
		|| !SecondSlots.IsValidIndex(ExchangeIndex))
	{
		return nullptr;
	}

	UWidget_BattleSkillRevealSlot* FirstSlot = FirstSlots[ExchangeIndex];

	UWidget_BattleSkillRevealSlot* SecondSlot = SecondSlots[ExchangeIndex];

	if (FirstSlot && FirstSlot->IsPlayerSkill() == bPlayerAction)
	{
		return FirstSlot;
	}

	if (SecondSlot && SecondSlot->IsPlayerSkill() == bPlayerAction)
	{
		return SecondSlot;
	}

	return nullptr;
}

void UWidget_BattleSkillRevealPanel::HandleSlotDeceiveRevealFinished(UWidget_BattleSkillRevealSlot* FinishedSlot)
{
	if (!IsValid(FinishedSlot) || FinishedSlot != PendingDeceiveSlot.Get())
	{
		return;
	}

	FinishedSlot->OnDeceiveRevealFinished.RemoveAll(this);

	// 완료 통지로 다음 Action이 시작될 수 있으므로 먼저 정리
	PendingDeceiveSlot = nullptr;

	OnDeceiveRevealFinished.Broadcast();
}

void UWidget_BattleSkillRevealPanel::HandleSlotHovered(UMuksiBattleCardDataAsset* SkillData, bool bIsPlayerSkill)
{
	OnSkillHovered.Broadcast(SkillData, bIsPlayerSkill);
}

void UWidget_BattleSkillRevealPanel::HandleSlotUnhovered(bool bIsPlayerSkill)
{
	OnSkillUnhovered.Broadcast(bIsPlayerSkill);
}

void UWidget_BattleSkillRevealPanel::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	// 1. 고정된 Border 영역 안에 마우스가 있는지 확인
	bHoverAreaHovered = false;

	if (Border_HoverArea && FSlateApplication::IsInitialized())
	{
		const FVector2D MousePosition =
			FSlateApplication::Get().GetCursorPos();

		bHoverAreaHovered = Border_HoverArea->GetCachedGeometry().IsUnderLocation(MousePosition);
	}

	if (!Overlay_Slots)
	{
		return;
	}

	// 2. 공개 중이거나 마우스가 근처에 있으면 내려온 상태 유지
	const bool bShouldStayDown = bRevealPresentationRunning || bHoverAreaHovered;

	const float DownY = BaseSlotsTranslation.Y + SlideDownDistance;

	const float TargetY = bShouldStayDown
		? DownY
		: BaseSlotsTranslation.Y;

	// 3. 현재 위치에서 목표 위치로 이동
	const FVector2D CurrentTranslation = Overlay_Slots->GetRenderTransform().Translation;

	const float NewY = FMath::FInterpConstantTo(
		CurrentTranslation.Y,
		TargetY,
		InDeltaTime,
		SlideSpeed
	);

	Overlay_Slots->SetRenderTranslation(FVector2D(CurrentTranslation.X, NewY));

	// 4. 내려온 위치에 도착한 뒤부터 유지 시간 계산
	if (bRevealPresentationRunning)
	{
		if (FMath::IsNearlyEqual(NewY, DownY, 0.1f))
		{
			RevealHoldElapsed += InDeltaTime;
		}

		TryFinishRevealPresentation();
	}
}

void UWidget_BattleSkillRevealPanel::TryFinishRevealPresentation()
{
	if (!bRevealPresentationRunning
		|| PendingRevealExchangeIndex == INDEX_NONE
		|| PendingRevealCount > 0
		|| RevealHoldElapsed < RevealHoldDuration)
	{
		return;
	}

	const int32 FinishedExchangeIndex =
		PendingRevealExchangeIndex;

	// 방송 전에 상태 정리
	bRevealPresentationRunning = false;
	RevealHoldElapsed = 0.0f;

	PendingRevealExchangeIndex = INDEX_NONE;
	PendingRevealCount = 0;

	OnSkillRevealFinished.Broadcast(FinishedExchangeIndex);
}
