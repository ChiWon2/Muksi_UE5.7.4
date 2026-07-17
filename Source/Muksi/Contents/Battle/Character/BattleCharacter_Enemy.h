// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "BattleCharacter_Enemy.generated.h"

class UEnemyBattleAIComponent;
class UMuksiCharacterDataAsset;
class ABattleManager;
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
	virtual void SetCharacterData(UMuksiCharacterDataAsset* InCharacterData) override;
	
	UMuksiBattleCardDataAsset* GetSelectEnemyCardDataAsset(ABattleGridManager* GridManager, ABattleManager* BattleManager)const;
	TArray<FIntPoint> GetSelectEnemyCardCoord()const;
	
	
	void InitData();
};
