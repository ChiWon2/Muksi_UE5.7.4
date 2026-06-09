// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBattleAIComponent.generated.h"

class UEnemyCardSelectStrategyBase;
class UCharacterDataBase;
class UMuksiBattleCardDataAsset;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UEnemyBattleAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyBattleAIComponent();
	
	void InitializeAI(UCharacterDataBase* InEnemyData);

	UMuksiBattleCardDataAsset* SelectCardsForExchange(
		UCharacterDataBase* EnemyData
	);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	UPROPERTY()
	TObjectPtr<UEnemyCardSelectStrategyBase> CardSelectStrategy;

		

};
