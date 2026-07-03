#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "../MuksiStatusEffectTypes.h"
#include "MuksiStatusEffectDeveloperSettings.generated.h"

UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Muksi Status Effect Settings"))
class MUKSI_API UMuksiStatusEffectDeveloperSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    UPROPERTY(Config, EditAnywhere, Category = "Status Effect")
    TArray<FStatusEffectRegistryData> StatusEffectRegistry;
};