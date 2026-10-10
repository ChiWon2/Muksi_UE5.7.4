// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "BattleCharacter_Enemy.generated.h"

class UMuksiBattleAIComponent;
class UMuksiCharacterDataAsset;
class ABattleGridManager;
struct FEnemySkillSelectResult;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|AI")
	TObjectPtr<UMuksiBattleAIComponent> BattleAIComponent = nullptr;
	virtual void SetCharacterData(UMuksiCharacterDataAsset* InCharacterData, ABattleManager* BattleManager) override;
	



	void InitData();
};
