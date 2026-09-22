// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/EnemyBattleAIComponent.h"

#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

// Sets default values for this component's properties
UEnemyBattleAIComponent::UEnemyBattleAIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UEnemyBattleAIComponent::InitializeAI(UMuksiCharacterDataAsset* InCharacterDataAsset)
{
	if (!InCharacterDataAsset)
	{
		return;
	}

	UMuksiCharacterDataAsset* CharacterAsset = InCharacterDataAsset;

	if (!CharacterAsset || !CharacterAsset->CardSelectStrategyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy AI StrategyClass is null"));
		return;
	}

	CardSelectStrategy = NewObject<UEnemyCardSelectStrategyBase>(
		this,
		CharacterAsset->CardSelectStrategyClass
	);
}


FEnemySkillSelectResult UEnemyBattleAIComponent::SelectSkillForExchange(const FCharacterData& EnemyData,
	const TArray<FBattleSkillInstance>& SkillInstances, ABattleGridManager* GridManager,
	const FHexOffsetCoord& EnemyCoord, const FHexOffsetCoord& PlayerCoord)
{
	if (!CardSelectStrategy)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CardSelectStrategy is null")
		);

		return FEnemySkillSelectResult();
	}

	if (SkillInstances.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Enemy SkillInstances is empty")
		);

		return FEnemySkillSelectResult();
	}

	SkillResult =
		CardSelectStrategy->SelectSkillForExchange(
			EnemyData,
			SkillInstances,
			GridManager,
			EnemyCoord,
			PlayerCoord
		);

	if (!SkillResult.SelectedSkill ||
		!SkillResult.SelectedSkillInstanceId.IsValid())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Enemy Skill Selection Failed")
		);

		return FEnemySkillSelectResult();
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[EnemyBattleAI] Skill Selected Skill=%s InstanceId=%s"),
		*GetNameSafe(SkillResult.SelectedSkill),
		*SkillResult.SelectedSkillInstanceId.ToString()
	);

	return SkillResult;
}





// Called when the game starts
void UEnemyBattleAIComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}




