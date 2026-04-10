// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/PlayerMode/PlayerMode_Battle.h"

#include "Controllers/MuksiPlayerController.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"

void UPlayerMode_Battle::EnterMode(AMuksiPlayerController* PlayerController)
{
	Super::EnterMode(PlayerController);
	
	PC->SetIgnoreLookInput(true);
	PC->SetIgnoreMoveInput(true);
	
	PC->bShowMouseCursor = true;
	PC->bEnableClickEvents = true;
	PC->bEnableMouseOverEvents = true;
	
	
	InitializeBattleTestData();
}

void UPlayerMode_Battle::ExitMode()
{
	Super::ExitMode();
	PC->SetIgnoreLookInput(false);
	PC->SetIgnoreMoveInput(false);
	
	PC->bShowMouseCursor = false;
	PC->bEnableClickEvents = false;
	PC->bEnableMouseOverEvents = false;
}


//***************** Test

void UPlayerMode_Battle::InitializeBattleTestData()
{
	if (!BattleCardDataTable || !TestPlayerCharacterDataAsset || !TestEnemyCharacterDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerMode::InitializeBattleTestData - Required asset is missing"));
		return;
	}

	PlayerCharacterData = NewObject<UCharacterDataBase>(this);
	EnemyCharacterData = NewObject<UCharacterDataBase>(this);

	if (PlayerCharacterData)
	{
		PlayerCharacterData->InitializeFromDataAsset(TestPlayerCharacterDataAsset, BattleCardDataTable);
	}

	if (EnemyCharacterData)
	{
		EnemyCharacterData->InitializeFromDataAsset(TestEnemyCharacterDataAsset, BattleCardDataTable);
	}
}


