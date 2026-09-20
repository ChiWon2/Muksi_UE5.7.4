#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "MuksiStatusEffect.generated.h"

struct FBattleAction;
struct FBattleExecutionContext;
struct FBattleExecutionEntry;

UCLASS(Abstract, BlueprintType)
class MUKSI_API UMuksiStatusEffect : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY()
    TObjectPtr<AActor> OwnerActor;

    UPROPERTY()
    FName EffectID = NAME_None;

    UPROPERTY()
    int32 CurrentStack = 1;

    UPROPERTY()
    int32 RemainingDuration = 1;

    UPROPERTY(EditDefaultsOnly, Category = "Battle Action")
    int32 BattleActionEditPriority = 0;

public:
    void Initialize(AActor* InOwnerActor,FName InEffectID,int32 InStackCount,int32 InDuration);

public:
    virtual void OnApplied();
    virtual void OnRemoved();

    virtual void OnReapplied(int32 AddedStack,int32 AddedDuration);

public:
	virtual void BuildPhaseExecutionEntries(EBattlePhase OldPhase, EBattlePhase NewPhase, TArray<FBattleExecutionEntry>& OutExecutionEntries);
	virtual void EditBattleActions(FBattleAction& CurrentAction, FBattleAction& OpponentAction);
	virtual void OnBattleActionCompleted(const FBattleAction& CompletedAction);
	virtual void OnBattleExchangeCompleted(int32 ExchangeIndex);
	virtual void BuildHitDealtExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries);
	virtual void BuildHitReceivedExecutionEntries(const FBattleExecutionContext& Context, int32 Damage, TArray<FBattleExecutionEntry>& OutExecutionEntries);

public:

    virtual bool IsExpired() const;

public:

    void AddStack(int32 Amount);

    void ConsumeStack(int32 Amount = 1);

    void ConsumeDuration(int32 Amount = 1);

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

    FORCEINLINE
        int32 GetBattleActionEditPriority() const
    {
        return BattleActionEditPriority;
    }
};
