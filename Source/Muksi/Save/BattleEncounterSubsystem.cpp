// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Save/BattleEncounterSubsystem.h"

#include "MuksiSaveSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

namespace BattleOccurPath
{
	//경로는 현재 위치
	//나중에 달라질 가능성 있음
	//캐릭터 폴더 위치 경로
	const FName CharacterDataFolder(
		TEXT("/Game/Battle/Data/BattleCharacters")
	);

	//레벨 폴더 위치 경로
	const FName BattleLevelFolder(
		TEXT("/Game/Maps/Battle")
	);

	const FString CharacterPrefix =
		TEXT("DA_");

	const FString BattleLevelPrefix =
		TEXT("L_Battle_");
}


void UBattleEncounterSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CacheCharacterDataPaths();
	CacheBattleLevelPaths();

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"BattleOccurSubsystem initialized. "
			"Characters=%d, Levels=%d"
		),
		CharacterDataPathMap.Num(),
		BattleLevelPathMap.Num()
	);
}

void UBattleEncounterSubsystem::Deinitialize()
{
	CharacterDataPathMap.Empty();
	BattleLevelPathMap.Empty();
	CurrentEnemyData = nullptr;
	
	Super::Deinitialize();
}

UBattleEncounterSubsystem* UBattleEncounterSubsystem::Get(const UObject* WorldContextObject)
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
		->GetSubsystem<UBattleEncounterSubsystem>();
}

bool UBattleEncounterSubsystem::StartBattleEncounter(FName BattleName)
{
	FName CharacterName;
    FName BattleLevelName;

    // CharacterName_LevelName
    // → CharacterName / LevelName
    if (!ParseBattleName(
        BattleName,
        CharacterName,
        BattleLevelName))
    {
        return false;
    }

	// CharacterName을 기준으로 CharacterDataAsset 가져오기
    UMuksiCharacterDataAsset* LoadedEnemyData =
        LoadCharacterData(CharacterName);

    if (!IsValid(LoadedEnemyData))
    {
        return false;
    }

	//LevelName을 기준으로 Map 가져오기
    if (!BattleLevelPathMap.Contains(BattleLevelName))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Battle level not found: %s"),
            *BattleLevelName.ToString()
        );

        return false;
    }

    UMuksiSaveSubsystem* SaveSubsystem =
        GetGameInstance()
        ->GetSubsystem<UMuksiSaveSubsystem>();

    if (!IsValid(SaveSubsystem))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("MuksiSaveSubsystem is invalid")
        );

        return false;
    }

    //전투 레벨 가기 전 현재 상태, 스탯, 월드 저장
    if (!SaveSubsystem->CaptureCurrentWorldState())
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Failed to capture current world state")
        );

        return false;
    }

    ReturnLevelName =
        FName(
            *UGameplayStatics::GetCurrentLevelName(
                this,
                true
            )
        );

    CurrentEncounterName = BattleName;
    CurrentEnemyData = LoadedEnemyData;

    

    if (!OpenBattleLevel(BattleLevelName))
    {
        CurrentEncounterName = NAME_None;
        //ReturnLevelName = NAME_None;
        CurrentEnemyData = nullptr;

        return false;
    }

    return true;
}

bool UBattleEncounterSubsystem::FinishBattleEncounter(const FBattleResult& BattleResult)
{
	if (ReturnLevelName.IsNone())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"FinishBattle: "
				"ReturnLevelName is None"
			)
		);

		return false;
	}
	

	UMuksiSaveSubsystem* SaveSubsystem =
		GetGameInstance()
		->GetSubsystem<UMuksiSaveSubsystem>();

	if (!IsValid(SaveSubsystem))
	{
		return false;
	}

	//SaveGame에 적기
	if (!SaveSubsystem->ApplyBattleResult(
		CurrentEncounterName,
		BattleResult))
	{
		return false;
	}

	//SaveGame을 SaveGameToSlot로 디스크에 적기
	if (!SaveSubsystem->WriteSaveGame())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"FinishBattle: "
				"Failed to write SaveGame"
			)
		);
		return false;
	}

	const FName TargetReturnLevel =
		ReturnLevelName;

	/*
	 * 월드가 돌아온 뒤 상태를 적용해야 한다는 표시.
	 */
	bPendingWorldRestore = true;

	UGameplayStatics::OpenLevel(
		this,
		TargetReturnLevel
	);

	return true;
}


void UBattleEncounterSubsystem::CacheCharacterDataPaths()
{
	CharacterDataPathMap.Empty();

	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			TEXT("AssetRegistry")
		);

	IAssetRegistry& AssetRegistry =
		AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;

	const bool bFoundAssets =
		AssetRegistry.GetAssetsByPath(
			BattleOccurPath::CharacterDataFolder,
			AssetDataList,
			true,
			true
		);

	if (!bFoundAssets)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"No character assets found in folder: %s"
			),
			*BattleOccurPath::CharacterDataFolder.ToString()
		);

		return;
	}
	for (const FAssetData& AssetData : AssetDataList)
	{
		if (!AssetData.IsInstanceOf(
			UMuksiCharacterDataAsset::StaticClass()))
		{
			continue;
		}

		FString CharacterNameString =
			AssetData.AssetName.ToString();

		CharacterNameString.RemoveFromStart(
			BattleOccurPath::CharacterPrefix
		);

		if (CharacterNameString.IsEmpty())
		{
			continue;
		}

		const FName CharacterName(
			*CharacterNameString
		);

		if (CharacterDataPathMap.Contains(CharacterName))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"Duplicate battle character name: %s"
				),
				*CharacterName.ToString()
			);

			continue;
		}

		CharacterDataPathMap.Add(
			CharacterName,
			AssetData.GetSoftObjectPath()
		);

		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"Cached battle character: %s -> %s"
			),
			*CharacterName.ToString(),
			*AssetData.GetSoftObjectPath().ToString()
		);
	}
}

void UBattleEncounterSubsystem::CacheBattleLevelPaths()
{
	BattleLevelPathMap.Empty();

	FAssetRegistryModule& AssetRegistryModule =
		FModuleManager::LoadModuleChecked<FAssetRegistryModule>(
			TEXT("AssetRegistry")
		);

	IAssetRegistry& AssetRegistry =
		AssetRegistryModule.Get();

	TArray<FAssetData> AssetDataList;

	const bool bFoundAssets =
		AssetRegistry.GetAssetsByPath(
			BattleOccurPath::BattleLevelFolder,
			AssetDataList,
			true,
			true
		);

	if (!bFoundAssets)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"No battle levels found in folder: %s"
			),
			*BattleOccurPath::BattleLevelFolder.ToString()
		);

		return;
	}

	for (const FAssetData& AssetData : AssetDataList)
	{
		if (!AssetData.IsInstanceOf(UWorld::StaticClass()))
		{
			continue;
		}

		FString LevelNameString =
			AssetData.AssetName.ToString();

		LevelNameString.RemoveFromStart(
			BattleOccurPath::BattleLevelPrefix
		);

		if (LevelNameString.IsEmpty())
		{
			continue;
		}

		const FName LevelName(*LevelNameString);

		if (BattleLevelPathMap.Contains(LevelName))
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT(
					"Duplicate battle level name: %s"
				),
				*LevelName.ToString()
			);

			continue;
		}

		BattleLevelPathMap.Add(
			LevelName,
			AssetData.GetSoftObjectPath()
		);

		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"Cached battle level: %s -> %s"
			),
			*LevelName.ToString(),
			*AssetData.GetSoftObjectPath().ToString()
		);
	}
}

bool UBattleEncounterSubsystem::ParseBattleName(FName BattleName, FName& OutCharacterName, FName& OutLevelName) const
{
	OutCharacterName = NAME_None;
	OutLevelName = NAME_None;

	if (BattleName.IsNone())
	{
		return false;
	}

	FString CharacterString;
	FString LevelString;

	const bool bSplit =
		BattleName.ToString().Split(
			TEXT("_"),
			&CharacterString,
			&LevelString,
			ESearchCase::CaseSensitive,
			ESearchDir::FromStart
		);

	if (!bSplit ||
		CharacterString.IsEmpty() ||
		LevelString.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"Invalid BattleName: %s. "
				"Expected Character_Level"
			),
			*BattleName.ToString()
		);

		return false;
	}

	OutCharacterName = FName(*CharacterString);
	OutLevelName = FName(*LevelString);

	return true;
}

UMuksiCharacterDataAsset* UBattleEncounterSubsystem::LoadCharacterData(FName CharacterName) const
{
	const FSoftObjectPath* FoundPath =
		CharacterDataPathMap.Find(CharacterName);

	if (!FoundPath)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"Character data path not found: %s"
			),
			*CharacterName.ToString()
		);

		return nullptr;
	}

	UObject* LoadedObject =
		FoundPath->ResolveObject();

	if (!IsValid(LoadedObject))
	{
		LoadedObject = FoundPath->TryLoad();
	}

	UMuksiCharacterDataAsset* CharacterData =
		Cast<UMuksiCharacterDataAsset>(LoadedObject);

	if (!IsValid(CharacterData))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"Failed to load character data: %s"
			),
			*FoundPath->ToString()
		);

		return nullptr;
	}

	return CharacterData;
}

bool UBattleEncounterSubsystem::OpenBattleLevel(FName LevelName)
{
	const FSoftObjectPath* FoundPath =
		BattleLevelPathMap.Find(LevelName);

	if (!FoundPath)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"Battle level path not found: %s"
			),
			*LevelName.ToString()
		);

		return false;
	}

	const FString PackageName =
		FoundPath->GetLongPackageName();

	if (PackageName.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"Invalid battle level package path: %s"
			),
			*FoundPath->ToString()
		);

		return false;
	}

	UGameplayStatics::OpenLevel(
		this,
		FName(*PackageName)
	);

	return true;
}
