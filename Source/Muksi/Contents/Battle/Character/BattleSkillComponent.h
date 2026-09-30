// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleSkillComponent.generated.h"

class UMuksiBattleCardDataAsset;
struct FBattleSkillInstance;

UENUM()
enum class EBattleSkillCostApplyType : uint8
{
	Player,
	Enemy
};

DECLARE_MULTICAST_DELEGATE(FOnBattleSkillStateChanged);
DECLARE_MULTICAST_DELEGATE(FOnBattleSkillCostChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBattleSkillComponent();

	//스킬 시스템---------------------------------------------------------------------------------------------------------
public:
	void Initialize(const TArray<UMuksiBattleCardDataAsset*>& InSkills, int32 InMaxSkillCost);

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
	int32 GetMaxSkillCost() const;
	
	int32 GetCurrentSkillCost() const;
	
	int32 GetDisplayedSkillCost() const;

	void RevealActualSkillCost(UMuksiBattleCardDataAsset* PresentedSkill);
	
	bool CanPaySkillCost(const FGuid& InstanceId) const;

	bool ConsumeSkillCost(const FGuid& InstanceId, EBattleSkillCostApplyType ApplyType);

	void RecoverSkillCost();
	
	FOnBattleSkillCostChanged OnBattleSkillCostChanged;
private:
	UPROPERTY(Transient)
	int32 CurrentSkillCost = 0;
	
	//변초 공개용
	UPROPERTY(Transient)
	int32 DisplayedSkillCost = 0;

	UPROPERTY(Transient)
	int32 MaxSkillCost = 10;
	//------------------------------------------------------------------------------------------------------------------
};
