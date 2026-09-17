// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleSkillSlot.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UWidget_BattleSkillSlot::SetSlotIndex(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
}

void UWidget_BattleSkillSlot::SetCardInstance(const FGuid& InInstanceId, UMuksiBattleCardDataAsset* InCardData)
{
	CardInstanceId = InInstanceId;
	CardData = InCardData;

	if (!CardData || !Image_SkillIcon)
	{
		return;
	}

	Image_SkillIcon->SetBrushFromTexture(CardData->CardIcon);
}

void UWidget_BattleSkillSlot::ClearCardInstance()
{
	CardInstanceId.Invalidate();
	CardData = nullptr;

	if (Image_SkillIcon)
	{
		Image_SkillIcon->SetBrushFromTexture(nullptr);
	}
}

void UWidget_BattleSkillSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Skill)
	{
		Button_Skill->OnClicked.AddUniqueDynamic(this,&UWidget_BattleSkillSlot::HandleSkillButtonClicked);
	}
}

void UWidget_BattleSkillSlot::HandleSkillButtonClicked()
{
	if (!CardData || !CardInstanceId.IsValid())
	{
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleSkillSlot] Click Slot=%d Card=%s"),
		SlotIndex,
		*GetNameSafe(CardData)
	);

	OnSkillSlotClicked.Broadcast(CardInstanceId,CardData);
}
