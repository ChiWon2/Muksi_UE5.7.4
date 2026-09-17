// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleSkillBar.h"

#include "Widget_BattleSkillSlot.h"
#include "Components/HorizontalBox.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"


void UWidget_BattleSkillBar::SetBattleCharacter(ABattleCharacterBase* InCharacter)
{
	BoundCharacter = InCharacter;

	RefreshSkillSlots();
}

void UWidget_BattleSkillBar::RefreshSkillSlots()
{
	if (!BoundCharacter)
	{
		return;
	}

	UBattleCardComponent* CardComponent = BoundCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return;
	}

	const TArray<FBattleCardInstance>& CurrentHand = CardComponent->GetCurrentHand();

	const int32 CardCount = FMath::Min(CurrentHand.Num(), SkillSlots.Num());

	for (int32 Index = 0; Index < CardCount; ++Index)
	{
		const FBattleCardInstance& CardInstance = CurrentHand[Index];

		if (!SkillSlots[Index])
		{
			continue;
		}
		
		UE_LOG(
	LogTemp,
	Warning,
	TEXT("[BattleSkillBar] Slot=%d Card=%s"),
	Index,
	*GetNameSafe(CardInstance.CardData)
);

		SkillSlots[Index]->SetCardInstance(CardInstance.InstanceId,CardInstance.CardData);
	}
}

bool UWidget_BattleSkillBar::ReturnCommittedSkill(const FGuid& InstanceId)
{
	if (!BoundCharacter || !InstanceId.IsValid())
	{
		return false;
	}

	UBattleCardComponent* CardComponent = BoundCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	if (!CardComponent->ReturnCommittedCard(InstanceId))
	{
		return false;
	}

	RefreshSkillSlots();

	return true;
}

void UWidget_BattleSkillBar::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeSkillSlots();
}

bool UWidget_BattleSkillBar::CommitSkillCard(const FGuid& InstanceId)
{
	if (!BoundCharacter)
	{
		return false;
	}

	UBattleCardComponent* CardComponent =
		BoundCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	return CardComponent->CommitCard(InstanceId);
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
		
		SkillSlot->OnSkillSlotClicked.AddUObject(this, &UWidget_BattleSkillBar::HandleSkillSlotClicked);
		
		SkillSlots.Add(SkillSlot);
	}
}

void UWidget_BattleSkillBar::HandleSkillSlotClicked(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData || !InstanceId.IsValid())
	{
		return;
	}
	
	if (!CommitSkillCard(InstanceId))
	{
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
