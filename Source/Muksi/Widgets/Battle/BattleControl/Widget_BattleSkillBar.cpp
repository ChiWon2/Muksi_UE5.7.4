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
	SetVisibility(
		bActive
			? ESlateVisibility::Visible
			: ESlateVisibility::HitTestInvisible
	);
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

		SkillSlots[Index]->SetCardInstance(SkillInstance.InstanceId,SkillInstance.SkillData, SkillInstance.RemainingCooldown);
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

void UWidget_BattleSkillBar::HandleSkillSlotClicked(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	if (!BoundCharacter || !CardData || !InstanceId.IsValid())
	{
		return;
	}

	UBattleSkillComponent* SkillComponent = BoundCharacter->GetBattleSkillComponent();

	if (!SkillComponent)
	{
		return;
	}

	if (!SkillComponent->CanUseSkill(InstanceId))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[BattleSkillBar] Skill is not ready. Card=%s"),
			*GetNameSafe(CardData)
		);

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillBar] Skill Clicked Card=%s"),
		*GetNameSafe(CardData)
	);

	OnBattleSkillSelected.Broadcast(InstanceId, CardData);
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
