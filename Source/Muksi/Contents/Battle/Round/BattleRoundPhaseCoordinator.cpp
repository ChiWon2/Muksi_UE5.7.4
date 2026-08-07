#include "Muksi/Contents/Battle/Round/BattleRoundPhaseCoordinator.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Flow/BattleAsyncStepRunner.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "TimerManager.h"

bool UBattleRoundPhaseCoordinator::Initialize(ABattleManager* InBattleManager)
{
    Shutdown();

    if (!IsValid(InBattleManager))
    {
        return false;
    }

    BattleManager = InBattleManager;
    StepRunner = NewObject<UBattleAsyncStepRunner>(this);
    if (!StepRunner)
    {
        BattleManager = nullptr;
        return false;
    }

    BattleManager->ChangePhaseDelegate.AddUniqueDynamic(
        this,
        &UBattleRoundPhaseCoordinator::HandleBattlePhaseChanged);
    return true;
}

void UBattleRoundPhaseCoordinator::Shutdown()
{
    if (BattleManager)
    {
        BattleManager->ChangePhaseDelegate.RemoveDynamic(
            this,
            &UBattleRoundPhaseCoordinator::HandleBattlePhaseChanged);
    }

    CancelRoundPhaseExecution();
    StepRunner = nullptr;
    BattleManager = nullptr;
}

void UBattleRoundPhaseCoordinator::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

void UBattleRoundPhaseCoordinator::HandleBattlePhaseChanged(
    EBattlePhase OldPhase,
    EBattlePhase NewPhase)
{
    static_cast<void>(OldPhase);

    if (NewPhase != EBattlePhase::RoundStart
        && NewPhase != EBattlePhase::RoundEnd)
    {
        if (StepRunner && StepRunner->IsRunning())
        {
            CancelRoundPhaseExecution();
        }
        return;
    }

    BeginRoundPhaseExecution(NewPhase);
}

void UBattleRoundPhaseCoordinator::BeginRoundPhaseExecution(EBattlePhase NewPhase)
{
    if (!StepRunner)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("[BattleRoundPhaseCoordinator] StepRunner is not initialized."));
        return;
    }

    if (StepRunner->IsRunning())
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[BattleRoundPhaseCoordinator] Ignored overlapping round phase execution. Phase=%d"),
            static_cast<int32>(NewPhase));
        return;
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager
        ? BattleManager->GetBattleRuntimeContext()
        : nullptr;
    if (!RuntimeContext)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("[BattleRoundPhaseCoordinator] BattleRuntimeContext is not available."));
        return;
    }

    const TWeakObjectPtr<ABattleCharacterBase> PlayerCharacter =
        RuntimeContext->GetPlayerCharacter();
    const TWeakObjectPtr<ABattleCharacterBase> EnemyCharacter =
        RuntimeContext->GetEnemyCharacter();

    TArray<FBattleAsyncStep> Steps;
    Steps.Reserve(4);

    Steps.Add([this, PlayerCharacter, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecutePassiveStep(PlayerCharacter.Get(), NewPhase, CompletionDelegate);
    });

    Steps.Add([this, EnemyCharacter, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecutePassiveStep(EnemyCharacter.Get(), NewPhase, CompletionDelegate);
    });

    Steps.Add([this, PlayerCharacter, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecuteStatusEffectStep(PlayerCharacter.Get(), NewPhase, CompletionDelegate);
    });

    Steps.Add([this, EnemyCharacter, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecuteStatusEffectStep(EnemyCharacter.Get(), NewPhase, CompletionDelegate);
    });

    StepRunner->Start(
        MoveTemp(Steps),
        FSimpleDelegate::CreateWeakLambda(this, [this, NewPhase]()
        {
            FinishRoundPhaseExecution(NewPhase);
        }));
}

void UBattleRoundPhaseCoordinator::ExecutePassiveStep(
    ABattleCharacterBase* Character,
    EBattlePhase NewPhase,
    const FSimpleDelegate& CompletionDelegate) const
{
    if (Character && Character->GetPassiveComponent())
    {
        Character->GetPassiveComponent()->ExecuteRoundPhaseSequentially(
            NewPhase,
            CompletionDelegate);
        return;
    }

    CompletionDelegate.ExecuteIfBound();
}

void UBattleRoundPhaseCoordinator::ExecuteStatusEffectStep(
    ABattleCharacterBase* Character,
    EBattlePhase Phase,
    const FSimpleDelegate& CompletionDelegate) const
{
    if (Character && Character->GetStatusEffectComponent())
    {
        Character->GetStatusEffectComponent()->ExecuteRoundPhaseSequentially(
            Phase,
            CompletionDelegate);
        return;
    }

    CompletionDelegate.ExecuteIfBound();
}

void UBattleRoundPhaseCoordinator::FinishRoundPhaseExecution(EBattlePhase FinishedPhase)
{
    if (!BattleManager || BattleManager->GetCurrentPhase() != FinishedPhase)
    {
        return;
    }

    BattleManager->GetWorldTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateWeakLambda(this, [this, FinishedPhase]()
        {
            if (BattleManager && BattleManager->GetCurrentPhase() == FinishedPhase)
            {
                BattleManager->NotifyPhaseExecutionFinished();
            }
        }));
}

void UBattleRoundPhaseCoordinator::CancelRoundPhaseExecution()
{
    if (StepRunner)
    {
        StepRunner->Cancel();
    }
}
