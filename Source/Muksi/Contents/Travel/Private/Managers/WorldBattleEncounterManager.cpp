#include "Muksi/Contents/Travel/Public/Managers/WorldBattleEncounterManager.h"

#include "Muksi/Contents/Battle/Encounter/BattleEncounterSubsystem.h"
#include "Muksi/Contents/Battle/Encounter/Data/MuksiBattleEncounterDataAsset.h"
#include "Kismet/GameplayStatics.h"

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

    return BattleSubsystem->StartBattleEncounter(EncounterID, *EncounterData);
}
