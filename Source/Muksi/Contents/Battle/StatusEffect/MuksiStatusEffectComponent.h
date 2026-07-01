#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MuksiStatusEffectTypes.h"
#include "MuksiStatusEffectComponent.generated.h"

class UMuksiStatusEffect;

DECLARE_MULTICAST_DELEGATE(FOnStatusEffectsChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiStatusEffectComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMuksiStatusEffectComponent();

private:
    UPROPERTY(VisibleAnywhere)
    TArray<TObjectPtr<UMuksiStatusEffect>> ActiveEffects;
public:
    FOnStatusEffectsChanged OnStatusEffectsChanged;

public:
    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* AddStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 1);

    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* SubtractStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 0);

    UFUNCTION(BlueprintCallable)
    void RemoveStatusEffect(FName EffectID);

public:
    UMuksiStatusEffect* FindEffectByID(FName EffectID) const;
    const TArray<TObjectPtr<UMuksiStatusEffect>>& GetActiveEffects() const;

private:
    void RemoveExpiredEffects();
    void RemoveStatusEffect(UMuksiStatusEffect* Effect);

public:
    void OnRoundStart();
    void OnExchangeStart();

    void OnAttackStart();
    void OnAttackEnd();

    void OnExchangeEnd();
    void OnRoundEnd();

};