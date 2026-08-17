#include "MuksiStatusEffect.h"

#include "MuksiStatusEffectComponent.h"

void UMuksiStatusEffect::BeginDestroy()
{
    CompleteExecution();
    Super::BeginDestroy();
}

void UMuksiStatusEffect::Initialize(AActor* InOwnerActor,UMuksiStatusEffectComponent* InOwnerComponent,FName InEffectID,int32 InStackCount,int32 InDuration)
{
    OwnerActor = InOwnerActor;
    OwnerComponent = InOwnerComponent;

    EffectID = InEffectID;

    CurrentStack = FMath::Max(1, InStackCount);
    RemainingDuration = FMath::Max(1, InDuration);
}

void UMuksiStatusEffect::CopyRuntimeStateFrom(const UMuksiStatusEffect& SourceEffect, AActor* InOwnerActor, UMuksiStatusEffectComponent* InOwnerComponent)
{
    CompleteExecution();
    OwnerActor = InOwnerActor;
    OwnerComponent = InOwnerComponent;
    EffectID = SourceEffect.EffectID;
    CurrentStack = SourceEffect.CurrentStack;
    RemainingDuration = SourceEffect.RemainingDuration;
}

void UMuksiStatusEffect::Execute(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion)
{
    if (bExecutionActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MuksiStatusEffect] Execution is already active."));
        CompleteExecution();
        return;
    }

    bExecutionActive = true;
    HandleExecute(OldPhase, NewPhase, bAllowDeferredCompletion);
}

void UMuksiStatusEffect::HandleExecute(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion)
{
    static_cast<void>(OldPhase);
    static_cast<void>(bAllowDeferredCompletion);

    switch (NewPhase)
    {
    case EBattlePhase::RoundStart:
        OnRoundStart();
        break;
    case EBattlePhase::ExchangeStart:
        OnExchangeStart();
        break;
    case EBattlePhase::BattleActionSequenceStart:
        OnBattleActionSequenceStart();
        break;
    case EBattlePhase::BattleActionSequenceEnd:
        OnBattleActionSequenceEnd();
        break;
    case EBattlePhase::ExchangeEnd:
        OnExchangeEnd();
        break;
    case EBattlePhase::RoundEnd:
        OnRoundEnd();
        break;
    default:
        break;
    }

    CompleteExecution();
}

void UMuksiStatusEffect::CompleteExecution()
{
    if (!bExecutionActive)
    {
        return;
    }

    bExecutionActive = false;
    if (IsValid(OwnerComponent)) OwnerComponent->NotifyStatusEffectExecutionFinished(this);
}

void UMuksiStatusEffect::OnApplied()
{
}

void UMuksiStatusEffect::OnRemoved()
{
}

void UMuksiStatusEffect::OnReapplied(int32 AddedStack,int32 AddedDuration)
{
    AddStack(AddedStack);

    RemainingDuration = FMath::Max(RemainingDuration , AddedDuration);
}

void UMuksiStatusEffect::OnRoundStart()
{
}

void UMuksiStatusEffect::OnExchangeStart()
{
}

void UMuksiStatusEffect::OnBattleActionSequenceStart()
{
}

void UMuksiStatusEffect::OnBattleActionStart(const FBattleAction& BattleAction)
{
    static_cast<void>(BattleAction);
}

void UMuksiStatusEffect::OnBattleActionSequenceEnd()
{
}

void UMuksiStatusEffect::OnExchangeEnd()
{
}

void UMuksiStatusEffect::OnRoundEnd()
{
}

bool UMuksiStatusEffect::IsExpired() const
{
    return RemainingDuration <= 0 || CurrentStack <= 0;
}

void UMuksiStatusEffect::AddStack(int32 Amount)
{
    CurrentStack += Amount;
}

void UMuksiStatusEffect::ConsumeStack(int32 Amount)
{
    CurrentStack = FMath::Max(0, CurrentStack - Amount);
}

void UMuksiStatusEffect::SetStack(int32 NewStack)
{
    CurrentStack = FMath::Max(0, NewStack);
}

void UMuksiStatusEffect::AddDuration(int32 Amount)
{
    RemainingDuration += Amount;
}

void UMuksiStatusEffect::ConsumeDuration(int32 Amount)
{
    RemainingDuration = FMath::Max(0, RemainingDuration - Amount);
}

void UMuksiStatusEffect::SetDuration(int32 NewDuration)
{
    RemainingDuration = FMath::Max(0, NewDuration);
}
