// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/MuksiGameMode.h"
#include "MuksiWorldGameMode.generated.h"

class APlayerController;
class AMuksiWorldCharacter;
class UMuksiSaveSubsystem;
class UMuksiSaveGame;

/**
 * 
 */
UCLASS()
class MUKSI_API AMuksiWorldGameMode : public AMuksiGameMode
{
	GENERATED_BODY()
protected:

	virtual void HandleStartingNewPlayer_Implementation(
		APlayerController* NewPlayer
	) override;
	
	virtual void BeginPlay() override;

private:

	bool RestoreWorldPlayer(
		APlayerController* PlayerController
	);


	bool CanRestoreSavedWorld(
		const UMuksiSaveGame* SaveGame
	) const;
	
	
	
	//Battle Encounter Test 용도임 제대로 작동하는거 보고 아래 전부 지워야 함
	void StartTestBattle();
	FTimerHandle TestBattleTimerHandle;
	
	UPROPERTY(EditDefaultsOnly, Category = "Test|Battle")
	FName TestBattleName = TEXT("SaveTestEnemy_TestMap");
	UPROPERTY(EditDefaultsOnly, Category = "Test|Battle") //BattleEncounter 테스트 진행 할건지
	bool bEnableAutoBattleTest = true;
	
	UPROPERTY(EditDefaultsOnly, Category = "Test|Battle", meta = (ClampMin = "0.0"))//몇초 후에 진행 할건지
	float TestBattleDelay = 5.0f;
};
