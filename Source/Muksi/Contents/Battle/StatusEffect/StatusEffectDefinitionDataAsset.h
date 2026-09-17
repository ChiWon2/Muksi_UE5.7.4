#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatusEffectDefinitionDataAsset.generated.h"

class UMuksiStatusEffect;
class UTexture2D;

UCLASS(BlueprintType)
class MUKSI_API UStatusEffectDefinitionDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect")
    TSoftClassPtr<UMuksiStatusEffect> EffectClass;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|Display")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|Display")
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|Display")
    TSoftObjectPtr<UTexture2D> Icon;
};
