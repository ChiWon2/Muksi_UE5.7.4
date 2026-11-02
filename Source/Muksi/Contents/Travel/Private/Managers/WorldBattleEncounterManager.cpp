#include "Muksi/Contents/Travel/Public/Managers/WorldBattleEncounterManager.h"

#include "Muksi/Contents/Battle/Encounter/BattleEncounterSubsystem.h"
#include "Muksi/Contents/Battle/Encounter/Data/MuksiBattleEncounterDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/MuksiPlayerDataSubsystem.h"
#include "Muksi/Contents/Travel/Public/Components/Player/StatComponent.h"

AWorldBattleEncounterManager::AWorldBattleEncounterManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

AWorldBattleEncounterManager* AWorldBattleEncounterManager::Get(const UObject* WorldContextObject)
{
    if (!IsValid(WorldContextObject))
        return nullptr;

    UWorld* World = WorldContextObject->GetWorld();

    if (!IsValid(World))
        return nullptr;

    return Cast<AWorldBattleEncounterManager>(UGameplayStatics::GetActorOfClass(World, StaticClass()));
}

bool AWorldBattleEncounterManager::StartEncounter(FName EncounterID)
{
    if (EncounterID.IsNone())
    {
        UE_LOG(LogTemp, Error, TEXT("[WorldEncounter] EncounterID is None"));
        return false;
    }

    const FMuksiBattleEncounterData* EncounterData = nullptr;

    for (UMuksiBattleEncounterDataAsset* DataAsset : EncounterDataAssets)
    {
        if (!IsValid(DataAsset))
            continue;

        const FMuksiBattleEncounterData* FoundData = DataAsset->EncounterDatas.Find(EncounterID);

        if (!FoundData)
            continue;

        if (EncounterData)
        {
            UE_LOG(LogTemp, Error, TEXT("[WorldEncounter] Duplicate EncounterID: %s (DataAsset: %s)"), *EncounterID.ToString(), *GetNameSafe(DataAsset));
            return false;
        }

        EncounterData = FoundData;
    }

    if (!EncounterData)
    {
        UE_LOG(LogTemp, Error, TEXT("[WorldEncounter] EncounterID not found: %s"), *EncounterID.ToString());
        return false;
    }

    UBattleEncounterSubsystem* BattleSubsystem = UBattleEncounterSubsystem::Get(this);

    if (!IsValid(BattleSubsystem))
    {
        UE_LOG(LogTemp, Error, TEXT("[WorldEncounter] BattleEncounterSubsystem is invalid"));
        return false;
    }

    // 월드 플레이어 스탯 가져오기(확인 후 리펙토링)-----------------------------------------------------------------------------------------
    UMuksiPlayerDataSubsystem* PlayerDataSubsystem = UMuksiPlayerDataSubsystem::Get(this);

    if (!IsValid(PlayerDataSubsystem))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[WorldEncounter] PlayerDataSubsystem is invalid"));
        return false;
    }

    UStatComponent* WorldStat = PlayerDataSubsystem->GetPlayerStatComponent();

    if (!IsValid(WorldStat))
    {
        UE_LOG(LogTemp, Error,
            TEXT("[WorldEncounter] WorldStat is invalid"));
        return false;
    }

    if (WorldStat->GetMaxHP() <= 0.f)
    {
        UE_LOG(LogTemp, Error,
            TEXT("[WorldEncounter] Invalid world MaxHP"));
        return false;
    }

    // 전투 초기화 데이터 생성
    FBattleCharacterInitData InitData;

    // 월드 스탯 적용
    InitData.Stats.HP = WorldStat->GetMaxHP();
    InitData.Stats.Attack = WorldStat->GetAttackPower();
    InitData.Stats.Defense = WorldStat->GetDefensePower();

    InitData.CurrentHP = FMath::Clamp(
        WorldStat->GetCurrentHP(),
        0.f,
        InitData.Stats.HP
    );
    
    // 월드 캐릭터의 스킬과 패시브 적용
    InitData.Skills = WorldStat->GetBaseSkillDataAssets();
    InitData.PassiveClasses = WorldStat->GetBasePassives();
    
    // 전투 진입 데이터 보관
    BattleSubsystem->SetPlayerBattleInitData(InitData);

    UE_LOG(LogTemp, Log,
        TEXT("[WorldEncounter] Battle InitData: HP=%.1f/%.1f ATK=%.1f DEF=%.1f"),
        InitData.CurrentHP,
        InitData.Stats.HP,
        InitData.Stats.Attack,
        InitData.Stats.Defense
    );

    // 전투 시작
    if (!BattleSubsystem->StartBattleEncounter(
            EncounterID, *EncounterData))
    {
        BattleSubsystem->ClearPlayerBattleInitData();
        return false;
    }

    return true;
}
