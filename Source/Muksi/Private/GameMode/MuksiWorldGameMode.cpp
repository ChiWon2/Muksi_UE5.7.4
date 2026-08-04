// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/MuksiWorldGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Travel/Public/Characters/MuksiWorldCharacter.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"
#include "Muksi/Save/MuksiSaveGame.h"
#include "Muksi/Save/MuksiSaveSubsystem.h"


void AMuksiWorldGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	if (!RestoreWorldPlayer(NewPlayer))
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"MuksiWorldGameMode: "
				"Player started without saved world state"
			)
		);
	}
}

void AMuksiWorldGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	
	
	

	//아래 내용은 BattleEncounter 테스트 삼아 진행하는 내용
	//정상 진행 시 아래 내용 삭제할 것
	if (!bEnableAutoBattleTest)
	{
		return;
	}
	if (TestBattleName.IsNone())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[MuksiWorldGameMode] "
				"TestBattleName is None"
			)
		);

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[MuksiWorldGameMode] "
			"Test battle will start after %.1f seconds. "
			"BattleName=%s"
		),
		TestBattleDelay,
		*TestBattleName.ToString()
	);

	GetWorldTimerManager().SetTimer(
		TestBattleTimerHandle,
		this,
		&AMuksiWorldGameMode::StartTestBattle,
		TestBattleDelay,
		false
	);
}

bool AMuksiWorldGameMode::RestoreWorldPlayer(APlayerController* PlayerController)
{
	if (!IsValid(PlayerController))
	{
		return false;
	}

	APawn* PlayerPawn =
		PlayerController->GetPawn();

	
	if (!IsValid(PlayerPawn))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"RestorePlayerTransform: "
				"PlayerPawn is invalid"
			)
		);

		return false;
	}

	UGameInstance* GameInstance =
		GetGameInstance();

	if (!IsValid(GameInstance))
	{
		return false;
	}

	UMuksiSaveSubsystem* SaveSubsystem =
		GameInstance
		->GetSubsystem<UMuksiSaveSubsystem>();

	if (!IsValid(SaveSubsystem))
	{
		return false;
	}

	UMuksiSaveGame* SaveGame =
		SaveSubsystem->GetCurrentSaveGame();

	if (!IsValid(SaveGame))
	{
		return false;
	}

	const FTransform& SavedTransform =
		SaveGame
		->PlayerWorldData
		.PlayerTransform;

	PlayerPawn->SetActorTransform(
		SavedTransform,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"Player transform restored. "
			"Pawn=%s, Location=%s"
		),
		*GetNameSafe(PlayerPawn),
		*SavedTransform
			.GetLocation()
			.ToString()
	);

	return true;
}

bool AMuksiWorldGameMode::CanRestoreSavedWorld(const UMuksiSaveGame* SaveGame) const
{
	if (!IsValid(SaveGame))
	{
		return false;
	}

	/*
	 * 새 게임으로 생성된 SaveGame은
	 * SavedWorldName이 비어 있다.
	 */
	if (SaveGame->SavedWorldName.IsNone())
	{
		return false;
	}

	const FString CurrentLevelString =
		UGameplayStatics::GetCurrentLevelName(
			this,
			true
		);

	if (CurrentLevelString.IsEmpty())
	{
		return false;
	}

	const FName CurrentLevelName(
		*CurrentLevelString
	);

	/*
	 * 다른 월드에서 저장한 Transform을 현재 월드에 적용하면
	 * 잘못된 위치로 이동할 수 있으므로 같은 레벨인지 검사한다.
	 */
	if (CurrentLevelName != SaveGame->SavedWorldName)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"CanRestoreSavedWorld: "
				"Saved world does not match current world. "
				"Saved=%s, Current=%s"
			),
			*SaveGame->SavedWorldName.ToString(),
			*CurrentLevelName.ToString()
		);

		return false;
	}

	return true;
}

void AMuksiWorldGameMode::StartTestBattle()
{
	UBattleEncounterSubsystem* BattleSubsystem =
		UBattleEncounterSubsystem::Get(this);

	if (!IsValid(BattleSubsystem))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[MuksiWorldGameMode] "
				"BattleEncounterSubsystem is invalid"
			)
		);

		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[MuksiWorldGameMode] "
			"Starting test battle: %s"
		),
		*TestBattleName.ToString()
	);

	if (!BattleSubsystem->StartBattleEncounter(
			TestBattleName))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[MuksiWorldGameMode] "
				"Failed to start test battle: %s"
			),
			*TestBattleName.ToString()
		);
	}
}
