#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "MuksiStatusEffectComponent.generated.h"

class ABattleManager;
class UMuksiStatusEffect;
class UBattleExecutionRunner;
struct FBattleAction;
struct FBattleExecutionContext;
struct FBattleExecutionEntry;

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
	void ResetRuntimeState();

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
	void AppendBattleActionStartExecutions(const FBattleAction& BattleAction, TArray<FBattleExecutionEntry>& OutExecutions) const;
	void AppendHitDealtExecutions(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutions) const;
	void AppendHitReceivedExecutions(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutions) const;

    void ExecuteSequentially(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);

protected:
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UPROPERTY(VisibleAnywhere)
    TArray<TObjectPtr<UMuksiStatusEffect>> ActiveEffects;

    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    void RemoveExpiredEffects();
    void RemoveStatusEffect(UMuksiStatusEffect* Effect);
    void ExecuteNextStatusEffect();
    void ExecutePhaseExecutions(const TArray<FBattleExecutionEntry>& ExecutionEntries);
    void HandlePhaseExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
    void FinishExecution();

    UPROPERTY(Transient)
    TArray<TObjectPtr<UMuksiStatusEffect>> ExecutionQueue;

	UPROPERTY(Transient)
	TObjectPtr<UBattleExecutionRunner> PhaseExecutionRunner = nullptr;

    FSimpleDelegate ExecutionCompletionDelegate;
    EBattlePhase ExecutingOldPhase = EBattlePhase::None;
    EBattlePhase ExecutingNewPhase = EBattlePhase::None;
    int32 ExecutionIndex = INDEX_NONE;
    bool bExecuting = false;
};
