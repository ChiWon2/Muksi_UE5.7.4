#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatusEffectDefinitionDataAsset.generated.h"

class UMuksiStatusEffect;
class UTexture2D;

USTRUCT(BlueprintType)
struct FStatusEffectFXSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|FX")
	FName AppliedFXDataAssetKey = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|FX")
	bool bWaitForAppliedFX = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|FX")
	FName AuraFXDataAssetKey = NAME_None;
};

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

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status Effect|FX")
    FStatusEffectFXSettings FXSettings;
};
