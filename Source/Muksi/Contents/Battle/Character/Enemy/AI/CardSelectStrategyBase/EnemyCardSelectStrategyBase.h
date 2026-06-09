// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "EnemyCardSelectStrategyBase.generated.h"

class UMuksiBattleCardDataAsset;
class UCharacterDataBase;
/**
 * 
 */
UCLASS(Blueprintable)
class MUKSI_API UEnemyCardSelectStrategyBase : public UObject
{
	GENERATED_BODY()
	
	
public:
	UFUNCTION(BlueprintNativeEvent)
	UMuksiBattleCardDataAsset* SelectCardsForExchange(
		UCharacterDataBase* EnemyData
	);

	// 캐릭터 마다 드로우 알고리즘 짜기
	virtual UMuksiBattleCardDataAsset* SelectCardsForExchange_Implementation(
		UCharacterDataBase* EnemyData
	);
};
