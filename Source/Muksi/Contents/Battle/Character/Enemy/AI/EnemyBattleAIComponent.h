// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"
#include "Components/ActorComponent.h"
#include "EnemyBattleAIComponent.generated.h"

class UMuksiCharacterDataAsset;
class ABattleCharacterBase;
class UEnemyCardSelectStrategyBase;
class UCharacterDataBase;
class UMuksiBattleCardDataAsset;
class ABattleGridManager;
struct FBattleSkillInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UEnemyBattleAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyBattleAIComponent();

	void InitializeAI(UMuksiCharacterDataAsset* CharacterDataAsset);


	
	FEnemySkillSelectResult SelectSkillForExchange(
	const FCharacterData& EnemyData,
	const TArray<FBattleSkillInstance>& SkillInstances,
	ABattleGridManager* GridManager,
	const FHexOffsetCoord& EnemyCoord,
	const FHexOffsetCoord& PlayerCoord);


protected:
    FEnemySkillSelectResult SkillResult;
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	TObjectPtr<UEnemyCardSelectStrategyBase> CardSelectStrategy;



};
