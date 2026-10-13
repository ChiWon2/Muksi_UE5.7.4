// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Muksi/Contents/Battle/Encounter/Data/MuksiBattleEncounterData.h"
#include "BattleEncounterSubsystem.generated.h"


//전투 종료 후 반영 될 정보들
//내용이랑 위치가 확정되지 않아서 일단 여기에 작성
USTRUCT(BlueprintType)
struct FBattleResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bPlayerWon = false;

	UPROPERTY(BlueprintReadOnly)
	int32 PlayerRemainingHP = 0;
};


/**
 * 
 */
class UMuksiCharacterDataAsset;

UCLASS()
class MUKSI_API UBattleEncounterSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
		static UBattleEncounterSubsystem* Get(
		const UObject* WorldContextObject);


	UFUNCTION(BlueprintCallable, Category = "Battle Occur")
	bool StartBattleEncounter(FName EncounterID, const FMuksiBattleEncounterData& EncounterData);
	UFUNCTION(BlueprintCallable, Category = "Battle Occur")
	bool FinishBattleEncounter(const FBattleResult& BattleResult);


	UFUNCTION(BlueprintPure, Category = "Battle Occur")
	UMuksiCharacterDataAsset* GetCurrentEnemyData() const
	{
		return CurrentEnemyData;
	}

private:
    bool OpenBattleLevel(const TSoftObjectPtr<UWorld>& BattleLevel);

private:
UPROPERTY(Transient)
	TObjectPtr<UMuksiCharacterDataAsset> CurrentEnemyData = nullptr;
	
	UPROPERTY(Transient)
	bool bPendingWorldRestore = false;

	
	UPROPERTY(Transient)
	FName CurrentEncounterName = NAME_None;
	UPROPERTY(Transient)
	FName ReturnLevelName = NAME_None;
};
