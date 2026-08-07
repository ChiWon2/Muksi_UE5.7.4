// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "BattleCharacter_Enemy.generated.h"

class UEnemyBattleAIComponent;
class UMuksiCharacterDataAsset;
class ABattleGridManager;
/**
 *
 */
UCLASS()
class MUKSI_API ABattleCharacter_Enemy : public ABattleCharacterBase
{
	GENERATED_BODY()
public:
	ABattleCharacter_Enemy();

public:
	UEnemyBattleAIComponent* BattleAIComponent;
	virtual void SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager) override;

	UMuksiBattleCardDataAsset* SelectCardForExchange(ABattleGridManager* GridManager, const FHexOffsetCoord& EnemyCoord, const FHexOffsetCoord& PlayerCoord) const;
	TArray<FHexOffsetCoord> GetSelectedTargetingStepCoords() const;


	void InitData();
};
