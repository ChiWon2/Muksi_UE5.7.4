#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MuksiStatusEffectTypes.h"
#include "MuksiStatusEffectSubsystem.generated.h"

UCLASS()
class MUKSI_API UMuksiStatusEffectSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
private:
    UPROPERTY()
    TMap<FName, FStatusEffectRegistryData> RegistryMap;

public:
    static UMuksiStatusEffectSubsystem* Get(const UObject* WorldContextObject);

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
    const FStatusEffectRegistryData* FindRegistryData(FName EffectID) const;

    TSubclassOf<UMuksiStatusEffect> FindEffectClass(FName EffectID) const;
};