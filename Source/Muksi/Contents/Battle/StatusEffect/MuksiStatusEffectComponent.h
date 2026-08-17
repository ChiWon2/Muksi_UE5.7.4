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
    void CopyRuntimeStateFrom(const UMuksiStatusEffectComponent& SourceComponent);
    void NotifyBattleActionStart(const FBattleAction& BattleAction);

    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* AddStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 1);

    UFUNCTION(BlueprintCallable)
    UMuksiStatusEffect* SubtractStatusEffect(FName EffectID, int32 StackCount = 1, int32 Duration = 0);

    UFUNCTION(BlueprintCallable)
    void RemoveStatusEffectByID(FName EffectID);

public:
    UMuksiStatusEffect* FindEffectByID(FName EffectID) const;
    int32 GetEffectStackCount(FName EffectID) const;
    const TArray<TObjectPtr<UMuksiStatusEffect>>& GetActiveEffects() const;

    void ExecuteSequentially(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate, bool bAllowDeferredCompletion = true);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	friend class UMuksiStatusEffect;

    UPROPERTY(VisibleAnywhere)
    TArray<TObjectPtr<UMuksiStatusEffect>> ActiveEffects;

    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    void RemoveExpiredEffects();
    void RemoveStatusEffect(UMuksiStatusEffect* Effect);
    void ExecuteNextStatusEffect();
    void NotifyStatusEffectExecutionFinished(UMuksiStatusEffect* FinishedStatusEffect);
    void FinishExecution();

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMuksiStatusEffect>> ExecutionQueue;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiStatusEffect> ExecutingStatusEffect = nullptr;

    FSimpleDelegate ExecutionCompletionDelegate;
    EBattlePhase ExecutingOldPhase = EBattlePhase::None;
    EBattlePhase ExecutingNewPhase = EBattlePhase::None;
    int32 ExecutionIndex = INDEX_NONE;
    bool bAllowDeferredCompletion = true;
    bool bExecuting = false;
};
