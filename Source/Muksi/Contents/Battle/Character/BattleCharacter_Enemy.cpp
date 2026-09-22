// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"

#include "BattleSkillComponent.h"
#include "Enemy/AI/EnemyBattleAIComponent.h"

ABattleCharacter_Enemy::ABattleCharacter_Enemy()
{
	//카드선택 AI Component
	BattleAIComponent = CreateDefaultSubobject<UEnemyBattleAIComponent>(TEXT("BattleAIComponent"));


}

void ABattleCharacter_Enemy::SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager)
{
	Super::SetCharacterData(InCharacterData, BattleManager);
	InitData();
}



FEnemySkillSelectResult ABattleCharacter_Enemy::SelectSkillForExchange(ABattleGridManager* GridManager,
	const FHexOffsetCoord& EnemyCoord, const FHexOffsetCoord& PlayerCoord) const
{
	if (!BattleAIComponent)
	{
		return FEnemySkillSelectResult();
	}

	UBattleSkillComponent* SkillComponent =
		GetBattleSkillComponent();

	if (!SkillComponent)
	{
		return FEnemySkillSelectResult();
	}

	TArray<FBattleSkillInstance> UsableSkills;

	for (const FBattleSkillInstance& SkillInstance : SkillComponent->GetSkillInstances())
	{
		if (!SkillComponent->CanUseSkill(SkillInstance.InstanceId))
		{
			continue;
		}

		UsableSkills.Add(SkillInstance);
	}
	
	if (UsableSkills.IsEmpty())
	{
		FEnemySkillSelectResult Result;
		Result.State = EEnemySkillSelectState::NoUsableSkill;

		return Result;
	}

	return BattleAIComponent->SelectSkillForExchange(
		CharacterData,
		UsableSkills,
		GridManager,
		EnemyCoord,
		PlayerCoord
	);
}




void ABattleCharacter_Enemy::InitData()
{
	if (BattleAIComponent)
	{
		BattleAIComponent->InitializeAI(CharacterData.CharacterAsset);
	}else UE_LOG(LogTemp,Error, TEXT("Component Error"));
}
