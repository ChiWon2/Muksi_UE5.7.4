// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleSkillBar.h"

#include "Widget_BattleSkillSlot.h"
#include "Components/HorizontalBox.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Contents/Battle/Character/BattleSkillTypes.h"


void UWidget_BattleSkillBar::SetBattleCharacter(ABattleCharacterBase* InCharacter)
{
	BoundCharacter = InCharacter;

	RefreshSkillSlots();
}

void UWidget_BattleSkillBar::SetSkillSlotActive(bool bActive)
{
	bSkillSlotActive = bActive;

	SetVisibility(
		bActive
			? ESlateVisibility::Visible
			: ESlateVisibility::HitTestInvisible
	);
}

bool UWidget_BattleSkillBar::TrySelectSkillSlot(int32 SlotIndex)
{
	if (!bSkillSlotActive || !BoundCharacter)
		return false;

	UBattleSkillComponent* SkillComponent = BoundCharacter->GetBattleSkillComponent();

	if (!SkillComponent)
		return false;

	const TArray<FBattleSkillInstance>& SkillInstances = SkillComponent->GetSkillInstances();

	if (!SkillInstances.IsValidIndex(SlotIndex))
		return false;

	const FBattleSkillInstance& SkillInstance = SkillInstances[SlotIndex];

	return TrySelectSkill(SkillInstance.InstanceId, SkillInstance.SkillData);
}

void UWidget_BattleSkillBar::RefreshSkillSlots()
{
	if (!BoundCharacter)
	{
		return;
	}

	UBattleSkillComponent* SkillComponent = BoundCharacter->GetBattleSkillComponent();

	if (!SkillComponent)
	{
		return;
	}

	const TArray<FBattleSkillInstance>& SkillInstances = SkillComponent->GetSkillInstances();

	const int32 SkillCount = FMath::Min(SkillInstances.Num(), SkillSlots.Num());
	const int32 CurrentCost = SkillComponent->GetCurrentSkillCost();
	

	for (int32 Index = 0; Index < SkillCount; ++Index)
	{
		const FBattleSkillInstance& SkillInstance = SkillInstances[Index];

		if (!SkillSlots[Index])
		{
			continue;
		}

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleSkillBar] Slot=%d Skill=%s Cooldown=%d"),
			Index,
			*GetNameSafe(SkillInstance.SkillData),
			SkillInstance.RemainingCooldown
		);

		SkillSlots[Index]->SetCardInstance(SkillInstance.InstanceId,SkillInstance.SkillData, SkillInstance.RemainingCooldown, CurrentCost);
	}
}


void UWidget_BattleSkillBar::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeSkillSlots();
}



void UWidget_BattleSkillBar::InitializeSkillSlots()
{
	if (!HorizontalBox_SkillSlots)
	{
		return;
	}

	SkillSlots.Empty();

	const int32 ChildCount = HorizontalBox_SkillSlots->GetChildrenCount();

	for (int32 Index = 0; Index < ChildCount; ++Index)
	{
		UWidget_BattleSkillSlot* SkillSlot = Cast<UWidget_BattleSkillSlot>(HorizontalBox_SkillSlots->GetChildAt(Index));

		if (!SkillSlot)
		{
			continue;
		}

		SkillSlot->SetSlotIndex(Index);
		
		SkillSlot->OnSkillSlotClicked.RemoveAll(this);
		SkillSlot->OnSkillSlotClicked.AddUObject(this, &UWidget_BattleSkillBar::HandleSkillSlotClicked);
		
		SkillSlot->OnSkillSlotHovered.RemoveAll(this);
		SkillSlot->OnSkillSlotHovered.AddUObject(this, &UWidget_BattleSkillBar::HandleSkillSlotHovered);
		SkillSlot->OnSkillSlotUnhovered.RemoveAll(this);
		SkillSlot->OnSkillSlotUnhovered.AddUObject(this, &UWidget_BattleSkillBar::HandleSkillSlotUnhovered);
		
		SkillSlots.Add(SkillSlot);
	}
}

bool UWidget_BattleSkillBar::TrySelectSkill(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	if (!bSkillSlotActive || !BoundCharacter || !CardData || !InstanceId.IsValid())
		return false;

	UBattleSkillComponent* SkillComponent = BoundCharacter->GetBattleSkillComponent();

	if (!SkillComponent)
		return false;

	if (!SkillComponent->CanUseSkill(InstanceId))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSkillBar] Skill is not ready. Card=%s"), *GetNameSafe(CardData));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("[BattleSkillBar] Skill Selected Card=%s"), *GetNameSafe(CardData));

	OnBattleSkillSelected.Broadcast(InstanceId, CardData);
	return true;
}

void UWidget_BattleSkillBar::HandleSkillSlotClicked(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	TrySelectSkill(InstanceId, CardData);
}

void UWidget_BattleSkillBar::HandleSkillSlotHovered(UMuksiBattleCardDataAsset* SkillData,  int32 RemainingCooldown)
{
	if (!SkillData)
	{
		return;
	}

	OnBattleSkillHovered.Broadcast(SkillData, RemainingCooldown);
}

void UWidget_BattleSkillBar::HandleSkillSlotUnhovered()
{
	OnBattleSkillUnhovered.Broadcast();
}
