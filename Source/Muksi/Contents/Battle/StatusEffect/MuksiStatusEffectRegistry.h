#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MuksiStatusEffectRegistry.generated.h"

class UMuksiStatusEffect;
class UStatusEffectDefinitionDataAsset;
class UStatusEffectRegistryDataAsset;

UCLASS()
class MUKSI_API UMuksiStatusEffectRegistry : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(const UStatusEffectRegistryDataAsset* InRegistryDataAsset);
    UStatusEffectDefinitionDataAsset* FindDefinition(FName EffectID) const;
    TSubclassOf<UMuksiStatusEffect> FindEffectClass(FName EffectID) const;

private:
    UPROPERTY()
    TMap<FName, TSoftObjectPtr<UStatusEffectDefinitionDataAsset>> DefinitionMap;
};
