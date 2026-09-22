// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"

#include "BattleSkillTypes.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

// Sets default values for this component's properties
UBattleSkillComponent::UBattleSkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UBattleSkillComponent::Initialize(const TArray<UMuksiBattleCardDataAsset*>& InSkills)
{
	SkillInstances.Empty();

	for (int32 Index = 0; Index < InSkills.Num(); ++Index)
	{
		UMuksiBattleCardDataAsset* SkillData = InSkills[Index];

		if (!SkillData)
		{
			continue;
		}

		SkillInstances.Emplace(SkillData, Index);
	}

	CurrentSkillCost = MaxSkillCost;
}

const FBattleSkillInstance* UBattleSkillComponent::FindSkillById(const FGuid& InstanceId) const
{
	return SkillInstances.FindByPredicate(
	   [&InstanceId](const FBattleSkillInstance& Skill)
	   {
		   return Skill.InstanceId == InstanceId;
	   });
}

bool UBattleSkillComponent::CanUseSkill(const FGuid& InstanceId) const
{
	const FBattleSkillInstance* Skill = FindSkillById(InstanceId);

	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	if (!Skill->IsReady())
	{
		return false;
	}

	if (!CanPaySkillCost(InstanceId))
	{
		return false;
	}

	return true;
}

bool UBattleSkillComponent::StartCooldown(const FGuid& InstanceId)
{
	FBattleSkillInstance* SkillInstance =
		SkillInstances.FindByPredicate(
			[&InstanceId](const FBattleSkillInstance& Skill)
			{
				return Skill.InstanceId == InstanceId;
			});

	if (!SkillInstance || !SkillInstance->SkillData)
	{
		return false;
	}

	SkillInstance->RemainingCooldown = FMath::Max(0, SkillInstance->SkillData->Cooldown);
	OnBattleSkillStateChanged.Broadcast();

	return true;
}

void UBattleSkillComponent::ReduceCooldowns()
{
	bool bChanged = false;
	for (FBattleSkillInstance& SkillInstance : SkillInstances)
	{
		if (SkillInstance.RemainingCooldown <= 0)
		{
			continue;
		}

		SkillInstance.RemainingCooldown -= 1;
		bChanged = true;
	}
	
	if (bChanged)
	{
		OnBattleSkillStateChanged.Broadcast();
	}
}

TArray<UMuksiBattleCardDataAsset*> UBattleSkillComponent::GetSkillDataList() const
{
	TArray<UMuksiBattleCardDataAsset*> Result;
	
	Result.Reserve(SkillInstances.Num());

	for (const FBattleSkillInstance& SkillInstance : SkillInstances)
	{
		if (!IsValid(SkillInstance.SkillData))
		{
			continue;
		}

		Result.Add(SkillInstance.SkillData);
	}

	return Result;
}

int32 UBattleSkillComponent::GetCurrentSkillCost() const
{
	return CurrentSkillCost;
}

bool UBattleSkillComponent::CanPaySkillCost(const FGuid& InstanceId) const
{
	const FBattleSkillInstance* Skill = FindSkillById(InstanceId);

	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	return Skill->SkillData->Cost <= CurrentSkillCost;
}

bool UBattleSkillComponent::ConsumeSkillCost(const FGuid& InstanceId)
{
	const FBattleSkillInstance* Skill = FindSkillById(InstanceId);

	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	if (!CanPaySkillCost(InstanceId))
	{
		return false;
	}

	CurrentSkillCost -= Skill->SkillData->Cost;

	return true;
}

void UBattleSkillComponent::RestoreSkillCost()
{
	//일단 모든 Cost 회복
	CurrentSkillCost = MaxSkillCost;
}



