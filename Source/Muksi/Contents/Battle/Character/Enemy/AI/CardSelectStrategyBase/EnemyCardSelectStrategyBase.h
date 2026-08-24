// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "UObject/Object.h"
#include "EnemyCardSelectStrategyBase.generated.h"

struct FBattleCardInstance;
struct FCharacterData;
class UMuksiBattleCardDataAsset;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FEnemyCardSelectResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SelectedCard = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TArray<FHexOffsetCoord> TargetingStepCoords;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid SelectedCardInstanceId;
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
	FEnemyCardSelectResult SelectCardForExchange(
	const FCharacterData& EnemyData,
		const TArray<FBattleCardInstance>& CurrentHand,
		ABattleGridManager* GridManager,
		const FHexOffsetCoord& EnemyCoord,
		const FHexOffsetCoord& PlayerCoord
	);

	virtual FEnemyCardSelectResult SelectCardForExchange_Implementation(
	const FCharacterData& EnemyData,
		const TArray<FBattleCardInstance>& CurrentHand,
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
