// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardDataAsset.h"
#include "Engine/DataAsset.h"
#include "MuksiCharacterDataAsset.generated.h"

class UEnemyCardSelectStrategyBase;
class UMuksiBattleCardDataAsset;
class ABattleCharacterBase;


/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FName CharacterID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UTexture2D> CharacterIllustration = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	int32 MaxHP = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText FactionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (MultiLine = true))
	FText FactionDescription;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Passive")
	TArray<TObjectPtr<UMuksiPassiveDataAsset>> SelfPassives;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Passive")
	TArray<TObjectPtr<UMuksiPassiveDataAsset>> AllyPassives;*/
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Deck")
	TArray<UMuksiBattleCardDataAsset*> CharacterDeck;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|AI")
	TSubclassOf<UEnemyCardSelectStrategyBase> CardSelectStrategyClass;
	
public:
	//캐릭터 모델, 클래스 등등.
	//나중에 통합 클래스로 변경 예정 <- 나중에는 Player전용으로 만들예정 그 때에는 ABattleCharacter_Enemy로 변경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Model")
	TSubclassOf<ABattleCharacterBase> BattleCharacterClass = nullptr;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
	TMap<ECardAnimType, TObjectPtr<UAnimMontage>> AttackAnimationMap;
	
	

};
