#include "MuksiStatusEffectRegistry.h"

#include "MuksiStatusEffect.h"
#include "StatusEffectDefinitionDataAsset.h"
#include "StatusEffectRegistryDataAsset.h"

bool UMuksiStatusEffectRegistry::Initialize(const UStatusEffectRegistryDataAsset* InRegistryDataAsset)
{
    DefinitionMap.Reset();

    if (!InRegistryDataAsset)
    {
        UE_LOG(LogTemp, Error, TEXT("[StatusEffectRegistry] Registry DataAsset is nullptr."));
        return false;
    }

    bool bValidRegistry = true;

    for (const FStatusEffectRegistryEntry& Entry : InRegistryDataAsset->Entries)
    {
        if (Entry.EffectID.IsNone())
        {
            UE_LOG(LogTemp, Error, TEXT("[StatusEffectRegistry] Registry entry has an invalid EffectID."));
            bValidRegistry = false;
            continue;
        }

        if (Entry.Definition.IsNull())
        {
            UE_LOG(LogTemp, Error, TEXT("[StatusEffectRegistry] Definition is missing. EffectID: %s"), *Entry.EffectID.ToString());
            bValidRegistry = false;
            continue;
        }

        if (DefinitionMap.Contains(Entry.EffectID))
        {
            UE_LOG(LogTemp, Error, TEXT("[StatusEffectRegistry] Duplicated EffectID: %s"), *Entry.EffectID.ToString());
            bValidRegistry = false;
            continue;
        }

        DefinitionMap.Add(Entry.EffectID, Entry.Definition);
    }

    return bValidRegistry;
}

UStatusEffectDefinitionDataAsset* UMuksiStatusEffectRegistry::FindDefinition(FName EffectID) const
{
    if (EffectID.IsNone())
        return nullptr;

    const TSoftObjectPtr<UStatusEffectDefinitionDataAsset>* DefinitionReference = DefinitionMap.Find(EffectID);
    if (!DefinitionReference)
        return nullptr;

    return DefinitionReference->LoadSynchronous();
}

TSubclassOf<UMuksiStatusEffect> UMuksiStatusEffectRegistry::FindEffectClass(FName EffectID) const
{
    UStatusEffectDefinitionDataAsset* Definition = FindDefinition(EffectID);
    if (!Definition)
        return nullptr;

    return Definition->EffectClass.LoadSynchronous();
}
