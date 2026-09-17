#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatusEffectRegistryDataAsset.generated.h"

class UStatusEffectDefinitionDataAsset;

USTRUCT(BlueprintType)
struct MUKSI_API FStatusEffectRegistryEntry
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect")
    FName EffectID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect")
    TSoftObjectPtr<UStatusEffectDefinitionDataAsset> Definition;
};

UCLASS(BlueprintType)
class MUKSI_API UStatusEffectRegistryDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect")
    TArray<FStatusEffectRegistryEntry> Entries;
};
