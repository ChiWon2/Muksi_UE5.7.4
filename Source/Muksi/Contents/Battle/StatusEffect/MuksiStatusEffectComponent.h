#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "MuksiStatusEffectTypes.h"
#include "MuksiStatusEffectComponent.generated.h"

class ABattleManager;
class UMuksiStatusEffect;

DECLARE_MULTICAST_DELEGATE(FOnStatusEffectsChanged);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiStatusEffectComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UMuksiStatusEffectComponent();

public:
    FOnStatusEffectsChanged OnStatusEffectsChanged;

public:
    void Initialize(ABattleManager* InBattleManager);

    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* AddStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 1);

    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* SubtractStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 0);

    UFUNCTION(BlueprintCallable)
    void RemoveStatusEffect(FName EffectID);

public:
    UMuksiStatusEffect* FindEffectByID(FName EffectID) const;
    const TArray<TObjectPtr<UMuksiStatusEffect>>& GetActiveEffects() const;

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(VisibleAnywhere)
    TArray<TObjectPtr<UMuksiStatusEffect>> ActiveEffects;

    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    void RemoveExpiredEffects();
    void RemoveStatusEffect(UMuksiStatusEffect* Effect);

    UFUNCTION()
    void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

    void HandleRoundStart();
    void HandleExchangeStart();

    void HandleAttackStart();
    void HandleAttackEnd();

    void HandleExchangeEnd();
    void HandleRoundEnd();

};
