#include "MuksiStatusEffectSubsystem.h"

#include "Engine/GameInstance.h"
#include "DeveloperSettings/MuksiStatusEffectDeveloperSettings.h"
#include "MuksiStatusEffect.h"

UMuksiStatusEffectSubsystem* UMuksiStatusEffectSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject)
    {
        return nullptr;
    }

    UWorld* World = WorldContextObject->GetWorld();
    if (!World)
    {
        return nullptr;
    }

    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return nullptr;
    }

    return GameInstance->GetSubsystem<UMuksiStatusEffectSubsystem>();
}

void UMuksiStatusEffectSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    RegistryMap.Empty();

    const UMuksiStatusEffectDeveloperSettings* Settings =GetDefault<UMuksiStatusEffectDeveloperSettings>();

    if (!Settings)
    {
        return;
    }

    for (const FStatusEffectRegistryData& Data : Settings->StatusEffectRegistry)
    {
        if (Data.EffectID.IsNone())
        {
            UE_LOG(LogTemp, Warning,TEXT("[StatusEffectSubsystem] Invalid EffectID. Skipped."));
            continue;
        }

        if (!Data.EffectClass)
        {
            UE_LOG(LogTemp, Warning,TEXT("[StatusEffectSubsystem] EffectClass is null. EffectID: %s"),*Data.EffectID.ToString());
            continue;
        }

        if (RegistryMap.Contains(Data.EffectID))
        {
            UE_LOG(LogTemp, Warning,TEXT("[StatusEffectSubsystem] Duplicated EffectID: %s"),*Data.EffectID.ToString());
            continue;
        }

        RegistryMap.Add(Data.EffectID, Data);
    }

    UE_LOG(LogTemp, Warning,TEXT("[StatusEffectSubsystem] Registry Loaded. Count: %d"),RegistryMap.Num());
}

const FStatusEffectRegistryData* UMuksiStatusEffectSubsystem::FindRegistryData(FName EffectID) const
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    return RegistryMap.Find(EffectID);
}

TSubclassOf<UMuksiStatusEffect> UMuksiStatusEffectSubsystem::FindEffectClass(FName EffectID) const
{
    const FStatusEffectRegistryData* Data = FindRegistryData(EffectID);

    if (!Data)
    {
        return nullptr;
    }

    return Data->EffectClass;
}