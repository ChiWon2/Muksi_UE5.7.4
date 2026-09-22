// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/SkillReveal/Widget_BattleSkillRevealPanel.h"

#include "Widget_BattleSkillRevealSlot.h"

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
		}
	}

	for (UWidget_BattleSkillRevealSlot* Slot_ : SecondSlots)
	{
		if (Slot_)
		{
			Slot_->SetRenderOpacity(0.0f);
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

	PendingRevealCount--;

	if (PendingRevealCount > 0)
	{
		return;
	}

	const int32 FinishedExchangeIndex = PendingRevealExchangeIndex;

	PendingRevealExchangeIndex = INDEX_NONE;
	PendingRevealCount = 0;

	OnSkillRevealFinished.Broadcast(FinishedExchangeIndex);
}

bool UWidget_BattleSkillRevealPanel::PlayDeceiveReveal(int32 ExchangeIndex, bool bPlayerAction,
	UMuksiBattleCardDataAsset* ActualSkill)
{
	if (!ActualSkill)
	{
		return false;
	}

	UWidget_BattleSkillRevealSlot* RevealSlot = FindActionSlot(ExchangeIndex, bPlayerAction);

	if (!RevealSlot)
	{
		return false;
	}

	// TODO 변초 공개 연출 넣기


	RevealSlot->SetSkillData(ActualSkill, bPlayerAction);

	OnDeceiveRevealFinished.Broadcast();

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
