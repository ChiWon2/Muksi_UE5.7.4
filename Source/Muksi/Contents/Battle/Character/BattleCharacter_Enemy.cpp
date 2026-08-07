// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"

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

UMuksiBattleCardDataAsset* ABattleCharacter_Enemy::SelectCardForExchange(
	ABattleGridManager* GridManager,
	const FHexOffsetCoord& EnemyCoord,
	const FHexOffsetCoord& PlayerCoord) const
{
	return BattleAIComponent->SelectCardForExchange(CharacterData, GridManager, EnemyCoord, PlayerCoord);
}

TArray<FHexOffsetCoord> ABattleCharacter_Enemy::GetSelectedTargetingStepCoords() const
{
	return BattleAIComponent->GetSelectedTargetingStepCoords();
}


void ABattleCharacter_Enemy::InitData()
{
	if (BattleAIComponent)
	{
		BattleAIComponent->InitializeAI(CharacterData.CharacterAsset);
	}else UE_LOG(LogTemp,Error, TEXT("Component Error"));
}
