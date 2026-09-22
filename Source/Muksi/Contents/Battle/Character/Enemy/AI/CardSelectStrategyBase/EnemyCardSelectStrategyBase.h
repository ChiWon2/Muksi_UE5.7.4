// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Muksi/Contents/Battle/Character/BattleSkillTypes.h"
#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "UObject/Object.h"
#include "EnemyCardSelectStrategyBase.generated.h"

struct FBattleCardInstance;
struct FCharacterData;
class UMuksiBattleCardDataAsset;
class ABattleGridManager;

UENUM()
enum class EEnemySkillSelectState : uint8
{
	Failed,
	Selected,
	NoUsableSkill
};


USTRUCT(BlueprintType)
struct FEnemySkillSelectResult
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	EEnemySkillSelectState State = EEnemySkillSelectState::Failed;
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SelectedSkill = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FHexOffsetCoord> TargetingStepCoords;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid SelectedSkillInstanceId;
};


/**
 *
 */
UCLASS(Blueprintable)
class MUKSI_API UEnemyCardSelectStrategyBase : public UObject
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintNativeEvent)
	FEnemySkillSelectResult SelectSkillForExchange(
		const FCharacterData& EnemyData,
		const TArray<FBattleSkillInstance>& SkillInstances,
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& EnemyCoord,
		const FHexOffsetCoord& PlayerCoord
	);

	virtual FEnemySkillSelectResult SelectSkillForExchange_Implementation(
		const FCharacterData& EnemyData,
		const TArray<FBattleSkillInstance>& SkillInstances,
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& EnemyCoord,
		const FHexOffsetCoord& PlayerCoord
	);

protected:
	virtual TArray<FHexOffsetCoord> GetCandidateCoords(
		FCharacterData EnemyData,
		UMuksiBattleCardDataAsset* Card,
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& EnemyCoord
		, const FHexOffsetCoord& PlayerCoord
	);

	virtual float EvaluateCardCoord(
		UMuksiBattleCardDataAsset* Card,
		const FHexOffsetCoord& CandidateCoord,
		const FHexOffsetCoord& PlayerCoord,
		ABattleGridManager* GridManager
	);
};
