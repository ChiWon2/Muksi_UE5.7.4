// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Save/MuksiSaveSubsystem.h"

#include "BattleEncounterSubsystem.h"
#include "MuksiSaveGame.h"
#include "Kismet/GameplayStatics.h"

UMuksiSaveSubsystem* UMuksiSaveSubsystem::Get(const UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return nullptr;
	}

	UWorld* World =
		WorldContextObject->GetWorld();

	if (!IsValid(World))
	{
		return nullptr;
	}

	UGameInstance* GameInstance =
		World->GetGameInstance();

	if (!IsValid(GameInstance))
	{
		return nullptr;
	}

	return GameInstance
		->GetSubsystem<UMuksiSaveSubsystem>();
}

void UMuksiSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(
		LogTemp,
		Log,
		TEXT("MuksiSaveSubsystem initialized")
	);

	if (!LoadOrCreateSaveGame())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"MuksiSaveSubsystem: "
				"Failed to load or create SaveGame"
			)
		);
	}
}

void UMuksiSaveSubsystem::Deinitialize()
{
	CurrentSaveGame = nullptr;

	UE_LOG(
		LogTemp,
		Log,
		TEXT("MuksiSaveSubsystem deinitialized")
	);

	Super::Deinitialize();
}

bool UMuksiSaveSubsystem::StartNewGame()
{
	if (!DeleteCurrentSaveGame())
	{
		return false;
	}

	CurrentSaveGame =
		Cast<UMuksiSaveGame>(
			UGameplayStatics::CreateSaveGameObject(
				UMuksiSaveGame::StaticClass()
			)
		);

	if (!IsValid(CurrentSaveGame))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("StartNewGame: Failed to create SaveGame")
		);

		return false;
	}

	/*
	 * 새 게임 기본값 설정
	 *
	 * 예:
	 * CurrentSaveGame->CurrentLevelName = TEXT("Story_Level_01");
	 * CurrentSaveGame->PlayerHP = 100;
	 * CurrentSaveGame->PlayerTransform = InitialTransform;
	 * CurrentSaveGame->CompletedEvents.Empty();
	 */

	const bool bSaved =
		UGameplayStatics::SaveGameToSlot(
			CurrentSaveGame,
			SaveSlotName,
			UserIndex
		);

	if (!bSaved)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("StartNewGame: Failed to write initial SaveGame")
		);

		CurrentSaveGame = nullptr;
		return false;
	}

	return true;
}

bool UMuksiSaveSubsystem::CaptureCurrentWorldState()
{
	//CurrentSaveGame 확인 <- 없으면 에러
	if (!IsValid(CurrentSaveGame.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"CaptureCurrentWorldState: "
				"CurrentSaveGame is invalid"
			)
		);

		return false;
	}

	UWorld* World = GetWorld();

	if (!IsValid(World))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"CaptureCurrentWorldState: "
				"World is invalid"
			)
		);

		return false;
	}

	//지금 메인으로 쓰고 있는 캐릭터 
	APawn* PlayerPawn =
		UGameplayStatics::GetPlayerPawn(
			World,
			0
		);

	if (!IsValid(PlayerPawn))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"CaptureCurrentWorldState: "
				"PlayerPawn is invalid"
			)
		);

		return false;
	}

	//월드(유량)레벨 이름 저장
	const FString CurrentLevelName =
		UGameplayStatics::GetCurrentLevelName(
			World,
			true
		);

	if (CurrentLevelName.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"CaptureCurrentWorldState: "
				"CurrentLevelName is empty"
			)
		);

		return false;
	}

	CurrentSaveGame->SavedWorldName =
		FName(*CurrentLevelName);

	CurrentSaveGame
		->PlayerWorldData
		.PlayerTransform =
			PlayerPawn->GetActorTransform();
	
	
	//메인 캐릭터 Transform하고 월드(유량)레벨에 대한 정보만 이 MuksiSaveSubsystem에서 저장해주고 나머지는 알아서
	OnSaveRequested.Broadcast(CurrentSaveGame);
	
	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"World state captured: "
			"Level=%s, Location=%s"
		),
		*CurrentSaveGame
			->SavedWorldName
			.ToString(),

		*CurrentSaveGame
			->PlayerWorldData
			.PlayerTransform
			.GetLocation()
			.ToString()
	);

	return true;
}

bool UMuksiSaveSubsystem::LoadOrCreateSaveGame()
{
	//1. CurrentSaveGame이 있는지
	//2. 없으면 디스크에는 CurrentSaveGame이 있는지
	//3. 그마저도 없으면 새 SaveGame 만들기
	
	if (IsValid(CurrentSaveGame.Get()))
	{
		return true;
	}
	
	//지정한 저장 슬롯에 세이브 파일이 존재하는지 확인 <- 지금 CurrentSaveGame은 nullptr인데 디스크에 저장한적 있는지 확인
	const bool bSaveExists =
		UGameplayStatics::DoesSaveGameExist(
			SaveSlotName,
			UserIndex
		);
	
	//이미 있음 <- 이전에 저장한 데이터가 남아있음
	if (bSaveExists)
	{
		//그 저장한 데이터 가져와서 CurrentSaveGame에 반영
		USaveGame* LoadedObject =
			UGameplayStatics::LoadGameFromSlot(
				SaveSlotName,
				UserIndex
			);

		CurrentSaveGame =
			Cast<UMuksiSaveGame>(
				LoadedObject
			);

		//클래스 안맞는 에러
		if (!IsValid(CurrentSaveGame.Get()))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"LoadOrCreateSaveGame: "
					"Failed to load or cast SaveGame. "
					"Slot=%s"
				),
				*SaveSlotName
			);

			return false;
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"SaveGame loaded: "
				"Slot=%s, World=%s"
			),
			*SaveSlotName,
			*CurrentSaveGame
				->SavedWorldName
				.ToString()
		);

		return true;
	}

	//지금 디스크에 저장한 적 없고 CurrentSaveGame도 nullptr인 진짜 첫 실행인 경우
	USaveGame* NewSaveObject =
		UGameplayStatics::CreateSaveGameObject(
			UMuksiSaveGame::StaticClass()
		);
	
	//새 SaveGame 클래스 만들기
	CurrentSaveGame =
		Cast<UMuksiSaveGame>(
			NewSaveObject
		);

	if (!IsValid(CurrentSaveGame.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"LoadOrCreateSaveGame: "
				"Failed to create MuksiSaveGame"
			)
		);

		return false;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"New MuksiSaveGame object created. "
			"Slot=%s"
		),
		*SaveSlotName
	);

	return true;
}

bool UMuksiSaveSubsystem::WriteWorldSaveGame()
{
	if (!IsValid(CurrentSaveGame.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"WriteSaveGame: "
				"CurrentSaveGame is invalid"
			)
		);

		return false;
	}
	OnSaveRequested.Broadcast(CurrentSaveGame);
	const bool bSaveSucceeded =
		UGameplayStatics::SaveGameToSlot(
			CurrentSaveGame.Get(),
			SaveSlotName,
			UserIndex
		);
	if (!bSaveSucceeded)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"WriteSaveGame: "
				"SaveGameToSlot failed. "
				"Slot=%s"
			),
			*SaveSlotName
		);

		return false;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"SaveGame written: "
			"Slot=%s, World=%s, Location=%s"
		),
		*SaveSlotName,

		*CurrentSaveGame
			->SavedWorldName
			.ToString(),

		*CurrentSaveGame
			->PlayerWorldData
			.PlayerTransform
			.GetLocation()
			.ToString()
	);

	return true;
}

bool UMuksiSaveSubsystem::WriteSaveGame()
{
	if (!IsValid(CurrentSaveGame.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"WriteSaveGame: "
				"CurrentSaveGame is invalid"
			)
		);

		return false;
	}

	const bool bSaveSucceeded =
		UGameplayStatics::SaveGameToSlot(
			CurrentSaveGame.Get(),
			SaveSlotName,
			UserIndex
		);

	if (!bSaveSucceeded)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"WriteSaveGame: "
				"SaveGameToSlot failed. "
				"Slot=%s"
			),
			*SaveSlotName
		);

		return false;
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"SaveGame written: "
			"Slot=%s, World=%s, Location=%s"
		),
		*SaveSlotName,

		*CurrentSaveGame
			->SavedWorldName
			.ToString(),

		*CurrentSaveGame
			->PlayerWorldData
			.PlayerTransform
			.GetLocation()
			.ToString()
	);

	return true;
}

bool UMuksiSaveSubsystem::DoesSaveGameExist() const
{
	return UGameplayStatics::DoesSaveGameExist(
		SaveSlotName,
		UserIndex
	);
}

bool UMuksiSaveSubsystem::HasLoadedSaveGame() const
{
	return IsValid(CurrentSaveGame.Get());
}

bool UMuksiSaveSubsystem::ApplyBattleResult(FName EncounterName, const FBattleResult& BattleResult)
{
	if (!IsValid(CurrentSaveGame.Get()))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"ApplyBattleResult: "
				"CurrentSaveGame is invalid"
			)
		);

		return false;
	}

	CurrentSaveGame
		->PlayerWorldData
		.CurrentHP =
			FMath::Clamp(
				BattleResult.PlayerRemainingHP,
				0,
				CurrentSaveGame
					->PlayerWorldData
					.MaxHP
			);

	if (BattleResult.bPlayerWon)
	{
		
	}

	UE_LOG(LogTemp, Log,TEXT("Battle result applied: " "Encounter=%s, Won=%s, HP=%d"), *EncounterName.ToString(),BattleResult.bPlayerWon
			? TEXT("true")
			: TEXT("false"),
		CurrentSaveGame
			->PlayerWorldData
			.CurrentHP
	);

	return true;
}

bool UMuksiSaveSubsystem::DeleteCurrentSaveGame()
{
	if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex))
	{
		return true;
	}

	const bool bDeleted =
		UGameplayStatics::DeleteGameInSlot(SaveSlotName, UserIndex);

	if (!bDeleted)
	{
		UE_LOG(LogTemp, Error, TEXT("DeleteCurrentSaveGame failed. " "Slot=%s, UserIndex=%d"),*SaveSlotName, UserIndex);
		return false;
	}

	return true;
}
