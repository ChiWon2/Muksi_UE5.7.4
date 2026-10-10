// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"

#include "BattleSkillComponent.h"
#include "Enemy/AI/MuksiBattleAIComponent.h"

ABattleCharacter_Enemy::ABattleCharacter_Enemy()
{
	SkillInfoAnchorComponent->SetRelativeLocation(FVector(131.0f, 0.0f, 131.0f));

	//카드선택 AI Component
	BattleAIComponent = CreateDefaultSubobject<UMuksiBattleAIComponent>(TEXT("MuksiBattleAIComponent"));


}

void ABattleCharacter_Enemy::SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager)
{
	Super::SetCharacterData(InCharacterData, BattleManager);
	InitData();
}



void ABattleCharacter_Enemy::InitData()
{
    // BehaviorTree is configured in MuksiCharacterDataAsset.
}
