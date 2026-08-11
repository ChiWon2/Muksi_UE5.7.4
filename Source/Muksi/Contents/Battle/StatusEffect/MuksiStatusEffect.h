#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "MuksiStatusEffect.generated.h"

class UMuksiStatusEffectComponent;
struct FBattleAction;

UCLASS(Abstract, BlueprintType)
class MUKSI_API UMuksiStatusEffect : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TObjectPtr<AActor> OwnerActor;

    UPROPERTY()
    TObjectPtr<UMuksiStatusEffectComponent> OwnerComponent;

    UPROPERTY()
    FName EffectID = NAME_None;

    UPROPERTY()
    int32 CurrentStack = 1;

    UPROPERTY()
    int32 RemainingDuration = 1;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "StatusEffect|Execution")
    bool bWaitForManualRoundPhaseCompletion = false;

private:
    FSimpleDelegate RoundPhaseCompletionDelegate;
    bool bRoundPhaseExecutionActive = false;

public:
    virtual void BeginDestroy() override;
    virtual void Initialize(AActor* InOwnerActor,UMuksiStatusEffectComponent* InOwnerComponent,FName InEffectID,int32 InStackCount,int32 InDuration);
    virtual void CopyRuntimeStateFrom(const UMuksiStatusEffect& SourceEffect, AActor* InOwnerActor, UMuksiStatusEffectComponent* InOwnerComponent);

    void ExecuteRoundPhase(EBattlePhase Phase, FSimpleDelegate CompletionDelegate);

    UFUNCTION(BlueprintCallable, Category = "StatusEffect|Execution")
    void NotifyRoundPhaseExecutionFinished();

public:
    virtual void OnApplied();
    virtual void OnRemoved();

    virtual void OnReapplied(int32 AddedStack,int32 AddedDuration);
public:

    virtual void OnRoundStart();
    virtual void OnExchangeStart();

    virtual void OnBattleActionSequenceStart();
    virtual void OnBattleActionStart(const FBattleAction& BattleAction);
    virtual void OnBattleActionSequenceEnd();

    virtual void OnExchangeEnd();
    virtual void OnRoundEnd();

public:

    virtual bool IsExpired() const;

public:

    void AddStack(int32 Amount);

    void ConsumeStack(int32 Amount = 1);

    void SetStack(int32 NewStack);

public:

    void AddDuration(int32 Amount);

    void ConsumeDuration(int32 Amount = 1);

    void SetDuration(int32 NewDuration);

public:
    FORCEINLINE
        const FName& GetEffectID() const
    {
        return EffectID;
    }

    FORCEINLINE
        int32 GetCurrentStack() const
    {
        return CurrentStack;
    }

    FORCEINLINE
        int32 GetRemainingDuration() const
    {
        return RemainingDuration;
    }
};