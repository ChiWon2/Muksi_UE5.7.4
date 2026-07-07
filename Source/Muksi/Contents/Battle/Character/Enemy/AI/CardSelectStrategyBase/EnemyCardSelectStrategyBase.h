// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemyCardSelectStrategyBase.generated.h"

class UMuksiBattleCardDataAsset;
class UCharacterDataBase;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FEnemyCardSelectResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SelectedCard = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedCoord = FIntPoint::ZeroValue;
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> SelectedCoordArray;

	UPROPERTY(BlueprintReadOnly)
	bool bValid = false;
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
		UCharacterDataBase* EnemyData,
		ABattleGridManager* GridManager,
		const FIntPoint& EnemyCoord,
		const FIntPoint& PlayerCoord
	);

	virtual FEnemyCardSelectResult SelectCardForExchange_Implementation(
		UCharacterDataBase* EnemyData,
		ABattleGridManager* GridManager,
		const FIntPoint& EnemyCoord,
		const FIntPoint& PlayerCoord
	);

protected:
	virtual TArray<FIntPoint> GetCandidateCoords(
		UCharacterDataBase* EnemyData,
		UMuksiBattleCardDataAsset* Card,
		ABattleGridManager* GridManager,
		const FIntPoint& EnemyCoord
		, const FIntPoint& PlayerCoord
	);

	virtual float EvaluateCardCoord(
		UMuksiBattleCardDataAsset* Card,
		const FIntPoint& CandidateCoord,
		const FIntPoint& PlayerCoord,
		ABattleGridManager* GridManager
	);
};
