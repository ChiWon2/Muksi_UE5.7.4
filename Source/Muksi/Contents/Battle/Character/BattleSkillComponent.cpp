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

void UBattleSkillComponent::Initialize(const TArray<UMuksiBattleCardDataAsset*>& InSkills, int32 InMaxSkillCost)
{
	UE_LOG(LogTemp, Error, TEXT("SkillComponent Init"));
	SkillInstances.Empty();
	
	MaxSkillCost = InMaxSkillCost;

	for (int32 Index = 0; Index < InSkills.Num(); ++Index)
	{
		UMuksiBattleCardDataAsset* SkillData = InSkills[Index];

		if (!SkillData)
		{
			continue;
		}

		SkillInstances.Emplace(SkillData, Index);
	}

	CurrentSkillCost = 3;
	DisplayedSkillCost = 3;
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

int32 UBattleSkillComponent::GetMaxSkillCost() const
{
	return MaxSkillCost;
}

int32 UBattleSkillComponent::GetCurrentSkillCost() const
{
	return CurrentSkillCost;
}

int32 UBattleSkillComponent::GetDisplayedSkillCost() const
{
	return DisplayedSkillCost;
}

void UBattleSkillComponent::RevealActualSkillCost(UMuksiBattleCardDataAsset* PresentedSkill)
{
	if (!IsValid(PresentedSkill))
	{
		return;
	}

	UMuksiBattleCardDataAsset* ActualSkill = PresentedSkill;

	const int32 PresentedCost = PresentedSkill->Cost;
	const int32 ActualCost = ActualSkill->Cost;

	const int32 CostDifference = ActualCost - PresentedCost;

	DisplayedSkillCost -= CostDifference;

	OnBattleSkillCostChanged.Broadcast();
}

bool UBattleSkillComponent::CanPaySkillCost(const FGuid& InstanceId) const
{
	const FBattleSkillInstance* Skill = FindSkillById(InstanceId);

	if (!Skill || !Skill->SkillData)
	{
		return false;
	}

	UMuksiBattleCardDataAsset* CostSkill = Skill->SkillData;

	return CostSkill->Cost <= CurrentSkillCost;
}

bool UBattleSkillComponent::ConsumeSkillCost(const FGuid& InstanceId, EBattleSkillCostApplyType ApplyType)
{
	const FBattleSkillInstance* Skill = FindSkillById(InstanceId);

	if (!Skill || !Skill->SkillData)
	{
		return false;
	}
	UMuksiBattleCardDataAsset* ActualSkill = Skill->SkillData;
	UMuksiBattleCardDataAsset* PresentedSkill = ActualSkill->GetDeceivedCard();

	if (!IsValid(PresentedSkill))
	{
		PresentedSkill = ActualSkill;
	}
	
	const int32 ActualCost = ActualSkill->Cost;
	const int32 PresentedCost = PresentedSkill->Cost;
	
	if (ActualCost > CurrentSkillCost)
	{
		return false;
	}
	

	CurrentSkillCost -= ActualCost;
	
	switch (ApplyType)
	{
	case EBattleSkillCostApplyType::Player:
		// Player는 변초여도 실제 Cost를 바로 공개
		DisplayedSkillCost -= ActualCost;
		break;

	case EBattleSkillCostApplyType::Enemy:
		// Enemy는 현재 보이는 가짜 스킬 Cost를 공개
		DisplayedSkillCost -= PresentedCost;
		break;
	}

	OnBattleSkillCostChanged.Broadcast();

	return true;
}

void UBattleSkillComponent::RecoverSkillCost()
{
	//일단 모든 Cost 회복
	CurrentSkillCost += 3;
	DisplayedSkillCost += 3;
	
	if (CurrentSkillCost > MaxSkillCost) CurrentSkillCost = MaxSkillCost;
	if (DisplayedSkillCost > MaxSkillCost) DisplayedSkillCost = MaxSkillCost;
	//CurrentSkillCost += 1;
	OnBattleSkillCostChanged.Broadcast();
}



