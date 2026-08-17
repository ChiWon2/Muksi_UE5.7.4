#include "MuksiStatusEffectComponent.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "MuksiStatusEffect.h"
#include "MuksiStatusEffectSubsystem.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

UMuksiStatusEffectComponent::UMuksiStatusEffectComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiStatusEffectComponent::Initialize(ABattleManager* InBattleManager)
{
    if (BattleManager)
    {
        BattleManager->BattleActionStartDelegate.RemoveAll(this);
    }

    BattleManager = InBattleManager;

    if (BattleManager)
    {
        BattleManager->BattleActionStartDelegate.AddUObject(this, &UMuksiStatusEffectComponent::NotifyBattleActionStart);
    }
}

void UMuksiStatusEffectComponent::CopyRuntimeStateFrom(const UMuksiStatusEffectComponent& SourceComponent)
{
    FinishExecution();
    ActiveEffects.Reset();
    for (UMuksiStatusEffect* SourceEffect : SourceComponent.ActiveEffects)
    {
        if (!IsValid(SourceEffect)) continue;
        UMuksiStatusEffect* NewEffect = NewObject<UMuksiStatusEffect>(this, SourceEffect->GetClass());
        if (!IsValid(NewEffect)) continue;
        NewEffect->CopyRuntimeStateFrom(*SourceEffect, GetOwner(), this);
        ActiveEffects.Add(NewEffect);
    }
}

void UMuksiStatusEffectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    FinishExecution();

    if (BattleManager)
    {
        BattleManager->BattleActionStartDelegate.RemoveAll(this);
        BattleManager = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::AddStatusEffect(FName EffectID,int32 StackCount,int32 Duration)
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    StackCount = FMath::Max(1, StackCount);
    Duration = FMath::Max(1, Duration);

    UMuksiStatusEffectSubsystem* StatusEffectSubsystem = UMuksiStatusEffectSubsystem::Get(this);

    TSubclassOf<UMuksiStatusEffect> EffectClass = StatusEffectSubsystem->FindEffectClass(EffectID);

    if (!EffectClass)
    {
        UE_LOG(LogTemp, Error,TEXT("[StatusEffectComponent] Cannot find EffectClass. EffectID: %s"),*EffectID.ToString());
        return nullptr;
    }

    if (UMuksiStatusEffect* ExistingEffect = FindEffectByID(EffectID))
    {
        ExistingEffect->OnReapplied(StackCount, Duration);

        OnStatusEffectsChanged.Broadcast();

        return ExistingEffect;
    }

    UMuksiStatusEffect* NewEffect = NewObject<UMuksiStatusEffect>(this,EffectClass);

    NewEffect->Initialize(GetOwner(),this,EffectID,StackCount,Duration);

    ActiveEffects.Add(NewEffect);

    NewEffect->OnApplied();

    OnStatusEffectsChanged.Broadcast();

    return NewEffect;
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::SubtractStatusEffect(FName EffectID,int32 StackCount,int32 Duration)
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    UMuksiStatusEffect* Effect = FindEffectByID(EffectID);
    if (!Effect)
    {
        return nullptr;
    }

    if (StackCount > 0)
    {
        Effect->ConsumeStack(StackCount);
    }

    if (Duration > 0)
    {
        Effect->ConsumeDuration(Duration);
    }

    if (Effect->IsExpired())
    {
        RemoveStatusEffect(Effect);
        return nullptr;
    }

    OnStatusEffectsChanged.Broadcast();

    return Effect;
}

void UMuksiStatusEffectComponent::RemoveStatusEffectByID(FName EffectID)
{
    UMuksiStatusEffect* Effect =  FindEffectByID(EffectID);
    if (Effect == nullptr)
        return;
    RemoveStatusEffect(Effect);
}

void UMuksiStatusEffectComponent::RemoveStatusEffect(UMuksiStatusEffect* Effect)
{
    if (!Effect)
    {
        return;
    }

    Effect->OnRemoved();

    ActiveEffects.Remove(Effect);

    OnStatusEffectsChanged.Broadcast();
}

UMuksiStatusEffect* UMuksiStatusEffectComponent::FindEffectByID(FName EffectID) const
{
    if (EffectID.IsNone())
    {
        return nullptr;
    }

    for (UMuksiStatusEffect* Effect : ActiveEffects)
    {
        if (Effect && Effect->GetEffectID() == EffectID)
        {
            return Effect;
        }
    }
    return nullptr;
}

int32 UMuksiStatusEffectComponent::GetEffectStackCount(FName EffectID) const
{
    if (EffectID.IsNone())
    {
        return 0;
    }
    
    for (UMuksiStatusEffect* Effect : ActiveEffects)
    {
        if (Effect && Effect->GetEffectID() == EffectID)
        {
            return Effect->GetCurrentStack();
        }
    }
    return 0;
}

const TArray<TObjectPtr<UMuksiStatusEffect>>& UMuksiStatusEffectComponent::GetActiveEffects() const
{
    return ActiveEffects;
}

void UMuksiStatusEffectComponent::RemoveExpiredEffects()
{
    bool bRemovedAny = false;

    for (int32 Index = ActiveEffects.Num() - 1; Index >= 0; --Index)
    {
        UMuksiStatusEffect* Effect = ActiveEffects[Index];

        if (Effect && Effect->IsExpired())
        {
            Effect->OnRemoved();
            ActiveEffects.RemoveAt(Index);

            bRemovedAny = true;
        }
    }

    if (bRemovedAny)
    {
        OnStatusEffectsChanged.Broadcast();
    }
}

void UMuksiStatusEffectComponent::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
    if (GetOwner() != BattleAction.Attacker)
    {
        return;
    }

    const TArray<TObjectPtr<UMuksiStatusEffect>> EffectsSnapshot = ActiveEffects;
    for (UMuksiStatusEffect* Effect : EffectsSnapshot)
    {
        if (IsValid(Effect))
        {
            Effect->OnBattleActionStart(BattleAction);
        }
    }

    RemoveExpiredEffects();
}

void UMuksiStatusEffectComponent::ExecuteSequentially(EBattlePhase OldPhase, EBattlePhase NewPhase,FSimpleDelegate CompletionDelegate, bool bInAllowDeferredCompletion)
{
    if (bExecuting)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MuksiStatusEffectComponent] Execution is already active."));
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    bExecuting = true;
    ExecutingOldPhase = OldPhase;
    ExecutingNewPhase = NewPhase;
    ExecutionQueue = ActiveEffects;
    ExecutionIndex = 0;
    bAllowDeferredCompletion = bInAllowDeferredCompletion;
    ExecutionCompletionDelegate = MoveTemp(CompletionDelegate);
    ExecuteNextStatusEffect();
}

void UMuksiStatusEffectComponent::ExecuteNextStatusEffect()
{
    if (!bExecuting)
    {
        return;
    }

    while (ExecutionQueue.IsValidIndex(ExecutionIndex))
    {
        UMuksiStatusEffect* Effect = ExecutionQueue[ExecutionIndex++];
        if (!IsValid(Effect))
        {
            continue;
        }

        ExecutingStatusEffect = Effect;
        Effect->Execute(ExecutingOldPhase, ExecutingNewPhase, bAllowDeferredCompletion);
        return;
    }

    RemoveExpiredEffects();
    FinishExecution();
}

void UMuksiStatusEffectComponent::NotifyStatusEffectExecutionFinished(UMuksiStatusEffect* FinishedStatusEffect)
{
	if (!bExecuting || FinishedStatusEffect != ExecutingStatusEffect)
	{
		return;
	}

	ExecutingStatusEffect = nullptr;
    ExecuteNextStatusEffect();
}

void UMuksiStatusEffectComponent::FinishExecution()
{
    if (!bExecuting)
    {
        return;
    }

    bExecuting = false;
    ExecutingOldPhase = EBattlePhase::None;
    ExecutingNewPhase = EBattlePhase::None;
	ExecutingStatusEffect = nullptr;
    ExecutionIndex = INDEX_NONE;
    ExecutionQueue.Reset();
    bAllowDeferredCompletion = true;

    FSimpleDelegate CompletionDelegate = MoveTemp(ExecutionCompletionDelegate);
    ExecutionCompletionDelegate.Unbind();
    CompletionDelegate.ExecuteIfBound();
}


