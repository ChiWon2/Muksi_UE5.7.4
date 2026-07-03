#pragma once

#include "CoreMinimal.h"
#include"MuksiStatusEffectIDs.h"
#include "MuksiStatusEffectTypes.generated.h"


class UMuksiStatusEffect;
class UTexture2D;

USTRUCT(BlueprintType)
struct FStatusEffectRegistryData
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EffectID = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UMuksiStatusEffect> EffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> Icon = nullptr;
};