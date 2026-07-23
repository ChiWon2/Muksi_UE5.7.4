// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MuksiBattleCardDataAsset.h"
#include "Engine/DataAsset.h"
#include "MuksiCharacterDataAsset.generated.h"

class UEnemyCardSelectStrategyBase;
class UMuksiBattleCardDataAsset;
class ABattleCharacterBase;
class UCharacterPassive;


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

public:
	//캐릭터 스탯 관련
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")//캐릭터 체력
	float MaxHP = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")
	float AttackValue = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")
	float DefenseValue = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")//캐릭터 선후공 속도
	float CharacterSpeed = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")//캐릭터 카드 선택 시간-최대값
	float CardSelectTimeMax = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Stat")//캐릭터 카드 선택 시간-최솟값
	float CardSelectTimeMin = 1.f;
	
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character")
	FText FactionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character", meta = (MultiLine = true))
	FText FactionDescription;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Deck")
	TArray<UMuksiBattleCardDataAsset*> CharacterDeck;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|AI")
	TSubclassOf<UEnemyCardSelectStrategyBase> CardSelectStrategyClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Passive")
	TArray<TSubclassOf<UCharacterPassive>> CharacterPassiveClass;
	
public:
	//캐릭터 모델, 클래스 등등.
	//나중에 통합 클래스로 변경 예정 <- 나중에는 Player전용으로 만들예정 그 때에는 ABattleCharacter_Enemy로 변경
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|Model")
	TSubclassOf<ABattleCharacterBase> BattleCharacterClass = nullptr;
	

};
