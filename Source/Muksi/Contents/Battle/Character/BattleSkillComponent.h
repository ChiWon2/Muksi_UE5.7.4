// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleSkillComponent.generated.h"

class UMuksiBattleCardDataAsset;
struct FBattleSkillInstance;

DECLARE_MULTICAST_DELEGATE(FOnBattleSkillStateChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleSkillComponent();

	//스킬 시스템---------------------------------------------------------------------------------------------------------
public:
	void Initialize(const TArray<UMuksiBattleCardDataAsset*>& InSkills);

	const TArray<FBattleSkillInstance>& GetSkillInstances() const {return SkillInstances;}

	const FBattleSkillInstance* FindSkillById(const FGuid& InstanceId) const;

	bool CanUseSkill(const FGuid& InstanceId) const;

	bool StartCooldown(const FGuid& InstanceId);
	
	void ReduceCooldowns();
	
	FOnBattleSkillStateChanged OnBattleSkillStateChanged;
	
	TArray<UMuksiBattleCardDataAsset*> GetSkillDataList() const;
private:
	UPROPERTY(Transient)
	TArray<FBattleSkillInstance> SkillInstances;
	//------------------------------------------------------------------------------------------------------------------
	
	//Character가 가지고 있는 Cost 관리------------------------------------------------------------------------------------
public:
	int32 GetCurrentSkillCost() const;

	bool CanPaySkillCost(const FGuid& InstanceId) const;

	bool ConsumeSkillCost(const FGuid& InstanceId);

	void RestoreSkillCost();
private:
	UPROPERTY(Transient)
	int32 CurrentSkillCost = 0;

	UPROPERTY(Transient)
	int32 MaxSkillCost = 10;
	//------------------------------------------------------------------------------------------------------------------
};
