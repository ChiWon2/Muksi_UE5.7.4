#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "MuksiStatusEffectTypes.h"
#include "MuksiStatusEffectComponent.generated.h"

class ABattleManager;
class UMuksiStatusEffect;
struct FBattleAction;

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

    void ExecuteRoundPhaseSequentially(EBattlePhase Phase, FSimpleDelegate CompletionDelegate);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(VisibleAnywhere)
    TArray<TObjectPtr<UMuksiStatusEffect>> ActiveEffects;

    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    void RemoveExpiredEffects();
    void RemoveStatusEffect(UMuksiStatusEffect* Effect);
    void ExecuteNextRoundPhaseStatusEffect();
    void HandleRoundPhaseStatusEffectFinished();
    void FinishRoundPhaseExecution();

    UFUNCTION()
    void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

    void HandleBattleActionStart(const FBattleAction& BattleAction);
    void HandleExchangeStart();

    void HandleBattleActionSequenceStart();
    void HandleBattleActionSequenceEnd();

    void HandleExchangeEnd();

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMuksiStatusEffect>> RoundPhaseExecutionQueue;

    FSimpleDelegate RoundPhaseCompletionDelegate;
    EBattlePhase ExecutingRoundPhase = EBattlePhase::None;
    int32 RoundPhaseExecutionIndex = INDEX_NONE;
    bool bExecutingRoundPhase = false;
};
