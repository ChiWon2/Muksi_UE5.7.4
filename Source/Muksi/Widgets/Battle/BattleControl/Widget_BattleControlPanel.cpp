// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/BattleControl/Widget_BattleControlPanel.h"

#include "Widget_BattleSkillBar.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

void UWidget_BattleControlPanel::SetBattleCharacter(ABattleCharacterBase* InCharacter)
{
	if (!BattleSkillBar)
	{
		return;
	}

	BattleSkillBar->SetBattleCharacter(InCharacter);
}

void UWidget_BattleControlPanel::RefreshSkillSlots()
{
	if (!BattleSkillBar)
	{
		return;
	}

	BattleSkillBar->RefreshSkillSlots();
}

bool UWidget_BattleControlPanel::ReturnCommittedSkill(const FGuid& InstanceId)
{
	if (!BattleSkillBar)
	{
		return false;
	}

	return BattleSkillBar->ReturnCommittedSkill(InstanceId);
}

void UWidget_BattleControlPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (BattleSkillBar)
	{
		BattleSkillBar->OnBattleSkillSelected.AddUObject(this,&UWidget_BattleControlPanel::HandleBattleSkillSelected);
	}
}

void UWidget_BattleControlPanel::HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[BattleControlPanel] Skill Selected Card=%s"),
		*GetNameSafe(CardData)
	);
	
	OnBattleSkillSelected.Broadcast(InstanceId, CardData);
}
