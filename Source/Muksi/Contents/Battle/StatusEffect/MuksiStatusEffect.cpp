#include "MuksiStatusEffect.h"

#include "MuksiStatusEffectComponent.h"

void UMuksiStatusEffect::BeginDestroy()
{
    NotifyRoundPhaseExecutionFinished();
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

void UMuksiStatusEffect::ExecuteRoundPhase(
    EBattlePhase Phase,
    FSimpleDelegate CompletionDelegate)
{
    if (bRoundPhaseExecutionActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("[MuksiStatusEffect] Round phase execution is already active."));
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    if (Phase != EBattlePhase::RoundStart && Phase != EBattlePhase::RoundEnd)
    {
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    bRoundPhaseExecutionActive = true;
    RoundPhaseCompletionDelegate = MoveTemp(CompletionDelegate);

    if (Phase == EBattlePhase::RoundStart)
    {
        OnRoundStart();
    }
    else
    {
        OnRoundEnd();
    }

    if (!bWaitForManualRoundPhaseCompletion)
    {
        NotifyRoundPhaseExecutionFinished();
    }
}

void UMuksiStatusEffect::NotifyRoundPhaseExecutionFinished()
{
    if (!bRoundPhaseExecutionActive)
    {
        return;
    }

    bRoundPhaseExecutionActive = false;
    FSimpleDelegate CompletionDelegate = MoveTemp(RoundPhaseCompletionDelegate);
    RoundPhaseCompletionDelegate.Unbind();
    CompletionDelegate.ExecuteIfBound();
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

    RemainingDuration =FMath::Max(RemainingDuration,AddedDuration);
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