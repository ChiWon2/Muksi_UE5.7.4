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
    FinishRoundPhaseExecution();
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
    FinishRoundPhaseExecution();

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

void UMuksiStatusEffectComponent::RemoveStatusEffect(FName EffectID)
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

#define PROCESS_STATUS_EFFECT_EVENT(FuncName) \
for (UMuksiStatusEffect* Effect : ActiveEffects) \
{ \
    if (Effect) \
    { \
        Effect->FuncName(); \
    } \
} \
RemoveExpiredEffects();


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

void UMuksiStatusEffectComponent::NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    HandleBattlePhaseChanged(OldPhase, NewPhase);
}

void UMuksiStatusEffectComponent::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    switch (NewPhase)
    {
    case EBattlePhase::ReadyStart:
    case EBattlePhase::ReadyEnd:
        break;
    case EBattlePhase::ExchangeStart:
        HandleExchangeStart();
        break;

    case EBattlePhase::BattleActionSequenceStart:
        HandleBattleActionSequenceStart();
        break;

    case EBattlePhase::BattleActionSequenceEnd:
        HandleBattleActionSequenceEnd();
        break;

    case EBattlePhase::ExchangeEnd:
        HandleExchangeEnd();
        break;

    default:
        break;
    }
}

void UMuksiStatusEffectComponent::HandleExchangeStart()
{
    PROCESS_STATUS_EFFECT_EVENT(OnExchangeStart)
}

void UMuksiStatusEffectComponent::HandleBattleActionSequenceStart()
{
    PROCESS_STATUS_EFFECT_EVENT(OnBattleActionSequenceStart)
}

void UMuksiStatusEffectComponent::HandleBattleActionSequenceEnd()
{
    PROCESS_STATUS_EFFECT_EVENT(OnBattleActionSequenceEnd)
}

void UMuksiStatusEffectComponent::HandleExchangeEnd()
{
    PROCESS_STATUS_EFFECT_EVENT(OnExchangeEnd)
}


void UMuksiStatusEffectComponent::ExecuteRoundPhaseSequentially(
    EBattlePhase Phase,
    FSimpleDelegate CompletionDelegate)
{
    if (bExecutingRoundPhase)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MuksiStatusEffectComponent] Round phase execution is already active."));
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    if (Phase != EBattlePhase::RoundStart && Phase != EBattlePhase::RoundEnd)
    {
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    bExecutingRoundPhase = true;
    ExecutingRoundPhase = Phase;
    RoundPhaseExecutionQueue = ActiveEffects;
    RoundPhaseExecutionIndex = 0;
    RoundPhaseCompletionDelegate = MoveTemp(CompletionDelegate);
    ExecuteNextRoundPhaseStatusEffect();
}

void UMuksiStatusEffectComponent::ExecuteNextRoundPhaseStatusEffect()
{
    if (!bExecutingRoundPhase)
    {
        return;
    }

    while (RoundPhaseExecutionQueue.IsValidIndex(RoundPhaseExecutionIndex))
    {
        UMuksiStatusEffect* Effect = RoundPhaseExecutionQueue[RoundPhaseExecutionIndex++];
        if (!IsValid(Effect))
        {
            continue;
        }

        Effect->ExecuteRoundPhase(
            ExecutingRoundPhase,
            FSimpleDelegate::CreateUObject(
                this,
                &UMuksiStatusEffectComponent::HandleRoundPhaseStatusEffectFinished));
        return;
    }

    RemoveExpiredEffects();
    FinishRoundPhaseExecution();
}

void UMuksiStatusEffectComponent::HandleRoundPhaseStatusEffectFinished()
{
    ExecuteNextRoundPhaseStatusEffect();
}

void UMuksiStatusEffectComponent::FinishRoundPhaseExecution()
{
    if (!bExecutingRoundPhase)
    {
        return;
    }

    bExecutingRoundPhase = false;
    ExecutingRoundPhase = EBattlePhase::None;
    RoundPhaseExecutionIndex = INDEX_NONE;
    RoundPhaseExecutionQueue.Reset();

    FSimpleDelegate CompletionDelegate = MoveTemp(RoundPhaseCompletionDelegate);
    RoundPhaseCompletionDelegate.Unbind();
    CompletionDelegate.ExecuteIfBound();
}

#undef PROCESS_STATUS_EFFECT_EVENT


