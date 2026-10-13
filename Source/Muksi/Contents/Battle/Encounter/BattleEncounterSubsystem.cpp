#include "Muksi/Contents/Battle/Encounter/BattleEncounterSubsystem.h"

#include "Muksi/Save/MuksiSaveSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"

namespace BattleEncounterPath
{
    const FName WorldLevelPath(TEXT("/Game/Maps/Jinsu/L_World_ForTestDialogue"));
}

UBattleEncounterSubsystem* UBattleEncounterSubsystem::Get(const UObject* WorldContextObject)
{
    if (!IsValid(WorldContextObject))
        return nullptr;

    UWorld* World = WorldContextObject->GetWorld();

    if (!IsValid(World))
        return nullptr;

    UGameInstance* GameInstance = World->GetGameInstance();

    if (!IsValid(GameInstance))
        return nullptr;

    return GameInstance->GetSubsystem<UBattleEncounterSubsystem>();
}

bool UBattleEncounterSubsystem::StartBattleEncounter(FName EncounterID, const FMuksiBattleEncounterData& EncounterData)
{
    if (EncounterID.IsNone())
        return false;

    UMuksiCharacterDataAsset* LoadedEnemyData = EncounterData.EnemyCharacter.LoadSynchronous();

    if (!IsValid(LoadedEnemyData))
    {
        UE_LOG(LogTemp, Error, TEXT("[Encounter] Failed to load enemy character: %s"), *EncounterID.ToString());
        return false;
    }

    if (EncounterData.BattleLevel.IsNull() || !EncounterData.BattleLevel.ToSoftObjectPath().IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[Encounter] BattleLevel is unset: %s"), *EncounterID.ToString());
        return false;
    }

    UMuksiSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UMuksiSaveSubsystem>();

    if (!IsValid(SaveSubsystem))
    {
        UE_LOG(LogTemp, Error, TEXT("[Encounter] MuksiSaveSubsystem is invalid"));
        return false;
    }

    if (!SaveSubsystem->CaptureCurrentWorldState())
    {
        UE_LOG(LogTemp, Error, TEXT("[Encounter] Failed to capture current world state"));
        return false;
    }

    ReturnLevelName = FName(*UGameplayStatics::GetCurrentLevelName(this, true));
    CurrentEncounterName = EncounterID;
    CurrentEnemyData = LoadedEnemyData;

    if (!OpenBattleLevel(EncounterData.BattleLevel))
    {
        CurrentEncounterName = NAME_None;
        CurrentEnemyData = nullptr;
        ReturnLevelName = NAME_None;
        return false;
    }

    return true;
}

bool UBattleEncounterSubsystem::FinishBattleEncounter(const FBattleResult& BattleResult)
{
    if (ReturnLevelName.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("FinishBattle: ReturnLevelName is None"));
        return false;
    }

    UMuksiSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UMuksiSaveSubsystem>();

    if (!IsValid(SaveSubsystem))
        return false;

    if (!SaveSubsystem->ApplyBattleResult(CurrentEncounterName, BattleResult))
        return false;

    if (!SaveSubsystem->WriteSaveGame())
    {
        UE_LOG(LogTemp, Warning, TEXT("FinishBattle: Failed to write SaveGame"));
        return false;
    }

    bPendingWorldRestore = true;

    UGameplayStatics::OpenLevel(this, BattleEncounterPath::WorldLevelPath);
    return true;
}

bool UBattleEncounterSubsystem::OpenBattleLevel(const TSoftObjectPtr<UWorld>& BattleLevel)
{
    const FString PackageName = BattleLevel.ToSoftObjectPath().GetLongPackageName();

    if (PackageName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[Encounter] Invalid battle level path: %s"), *BattleLevel.ToString());
        return false;
    }

    UGameplayStatics::OpenLevel(this, FName(*PackageName));
    return true;
}
