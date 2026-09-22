// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/SkillReveal/Widget_BattleSkillRevealSlot.h"

#include "Components/Image.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UWidget_BattleSkillRevealSlot::SetSkillData(UMuksiBattleCardDataAsset* InSkillData, bool IsPlayer)
{
	SkillData = InSkillData;
	bIsPlayerSkill = IsPlayer;

	if (!Image_Skill)
	{
		return;
	}

	if (!SkillData)
	{
		ClearSkill();
		return;
	}

	Image_Skill->SetBrushFromTexture(SkillData->CardIcon);

	Image_Skill->SetVisibility(ESlateVisibility::HitTestInvisible);
	SetRenderOpacity(1.0f);
}

void UWidget_BattleSkillRevealSlot::ClearSkill()
{
	SkillData = nullptr;

	if (!Image_Skill)
	{
		return;
	}

	Image_Skill->SetBrushFromTexture(nullptr);

	Image_Skill->SetVisibility(ESlateVisibility::Collapsed);
}

bool UWidget_BattleSkillRevealSlot::PlayRevealAnimation()
{
	if (!SkillData)
	{
		return false;
	}

	SetRenderOpacity(1.0f);

	if (!Anim_Reveal)
	{
		return false;
	}

	PlayAnimation(Anim_Reveal);

	return true;
}

void UWidget_BattleSkillRevealSlot::NativeConstruct()
{
	Super::NativeConstruct();
	if (Anim_Reveal)
	{
		FWidgetAnimationDynamicEvent FinishedEvent;

		FinishedEvent.BindDynamic(
			this,
			&UWidget_BattleSkillRevealSlot::HandleRevealAnimationFinished
		);

		BindToAnimationFinished(
			Anim_Reveal,
			FinishedEvent
		);
	}
}

void UWidget_BattleSkillRevealSlot::HandleRevealAnimationFinished()
{
	OnRevealFinished.Broadcast(this);
}
