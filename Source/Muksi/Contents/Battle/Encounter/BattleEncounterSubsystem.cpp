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

bool UBattleEncounterSubsystem::ConsumePendingBattleReturn(FName& OutEncounterID, FBattleResult& OutBattleResult)
{
    if (!bPendingWorldRestore)
    {
        return false;
    }

    if (CurrentEncounterName.IsNone())
    {
        return false;
    }

    // 전투 복귀 정보 전달
    OutEncounterID = CurrentEncounterName;
    OutBattleResult = PendingBattleResult;

    // 중복 처리를 막기 위한 초기화
    bPendingWorldRestore = false;

    PendingBattleResult = FBattleResult();

    return true;
}

bool UBattleEncounterSubsystem::StartBattleEncounter(FName EncounterID, const FMuksiBattleEncounterData& EncounterData)
{
    if (EncounterID.IsNone())
        return false;

    UMuksiCharacterDataAsset* LoadedEnemyData = EncounterData.EnemyCharacter.LoadSynchronous();

    if (!IsValid(LoadedEnemyData))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleEncounterSubsystem.cpp] Failed to load enemy character: %s"), *EncounterID.ToString());
        return false;
    }

    if (EncounterData.BattleLevel.IsNull() || !EncounterData.BattleLevel.ToSoftObjectPath().IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleEncounterSubsystem.cpp] BattleLevel is unset: %s"), *EncounterID.ToString());
        return false;
    }

    UMuksiSaveSubsystem* SaveSubsystem = GetGameInstance()->GetSubsystem<UMuksiSaveSubsystem>();

    if (!IsValid(SaveSubsystem))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleEncounterSubsystem.cpp] MuksiSaveSubsystem is invalid"));
        return false;
    }

    if (!SaveSubsystem->WriteWorldSaveGame())
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleEncounterSubsystem.cpp] Failed to save current world state"));

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

    // 전투 결과 보관
    PendingBattleResult = BattleResult;

    // 월드 복귀 대기 상태
    bPendingWorldRestore = true;
    UE_LOG(LogTemp, Log, TEXT("[BattleEncounterSubsystem.cpp] Pending Battle Return: %s"), *CurrentEncounterName.ToString());

    // 월드로 복귀
    UGameplayStatics::OpenLevel(this, BattleEncounterPath::WorldLevelPath);
    return true;
}

void UBattleEncounterSubsystem::SetPlayerBattleInitData(const FBattleCharacterInitData& InitData)
{
    PendingPlayerInitData = InitData;
    bHasPendingPlayerInitData = true;
}

bool UBattleEncounterSubsystem::GetPlayerBattleInitData(FBattleCharacterInitData& OutInitData) const
{
    if (!bHasPendingPlayerInitData)
    {
        return false;
    }

    OutInitData = PendingPlayerInitData;
    return true;
}

void UBattleEncounterSubsystem::ClearPlayerBattleInitData()
{
    PendingPlayerInitData = FBattleCharacterInitData();
    bHasPendingPlayerInitData = false;
}

bool UBattleEncounterSubsystem::OpenBattleLevel(const TSoftObjectPtr<UWorld>& BattleLevel)
{
    const FString PackageName = BattleLevel.ToSoftObjectPath().GetLongPackageName();

    if (PackageName.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleEncounterSubsystem.cpp] Invalid battle level path: %s"), *BattleLevel.ToString());
        return false;
    }

    UGameplayStatics::OpenLevel(this, FName(*PackageName));
    return true;
}
