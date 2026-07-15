// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget.h"
#include "CharacterDataPanelWidget_Enemy.generated.h"

class UCharacterData_Enemy;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterDataPanelWidget_Enemy : public UCharacterDataPanelWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION()
	void ApplyEnemyData(ABattleCharacter_Enemy* InEnemyData);
	
private:
	UPROPERTY()
	ABattleCharacter_Enemy* EnemyData;
	
};
