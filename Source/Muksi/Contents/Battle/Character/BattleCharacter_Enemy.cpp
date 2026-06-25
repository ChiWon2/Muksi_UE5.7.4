// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"

#include "Enemy/AI/EnemyBattleAIComponent.h"

ABattleCharacter_Enemy::ABattleCharacter_Enemy()
{
	//카드선택 AI Component
	BattleAIComponent = CreateDefaultSubobject<UEnemyBattleAIComponent>(TEXT("BattleAIComponent"));
	
	
}

void ABattleCharacter_Enemy::SetCharacterData(UCharacterDataBase* InCharacterData)
{
	Super::SetCharacterData(InCharacterData);
	InitData();
}

UMuksiBattleCardDataAsset* ABattleCharacter_Enemy::GetSelectEnemyCardDataAsset() const
{
	return BattleAIComponent->SelectCardsForExchange(CharacterData);
}

void ABattleCharacter_Enemy::InitData()
{
	if (BattleAIComponent)
	{
		BattleAIComponent->InitializeAI(CharacterData);
	}else UE_LOG(LogTemp,Error, TEXT("Component Error"));
}
