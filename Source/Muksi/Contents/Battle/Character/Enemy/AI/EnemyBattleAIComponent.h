// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"
#include "Components/ActorComponent.h"
#include "EnemyBattleAIComponent.generated.h"

class UEnemyCardSelectStrategyBase;
class UCharacterDataBase;
class UMuksiBattleCardDataAsset;
class ABattleGridManager;
class ABattleManager;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UEnemyBattleAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEnemyBattleAIComponent();
	
	void InitializeAI(UCharacterDataBase* InEnemyData);

	UMuksiBattleCardDataAsset* SelectCardForExchange(
		UCharacterDataBase* EnemyData,
		ABattleGridManager* GridManager,
		ABattleManager* BattleManager
	);
	
	TArray<FIntPoint> GetPointForExchange()const;
	
protected:
	TArray<FEnemyCardSelectResult> SelectCardArray;
	
	FEnemyCardSelectResult Result;
	
protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY()
	TObjectPtr<UEnemyCardSelectStrategyBase> CardSelectStrategy;

		

};
