#include "Muksi/Contents/Battle/Flow/BattleCharacterPhaseCoordinator.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Flow/BattleAsyncStepRunner.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "TimerManager.h"

bool UBattleCharacterPhaseCoordinator::Initialize(ABattleManager* InBattleManager)
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

    BattleManager->PhaseUIFinishedDelegate.AddUObject(this, &UBattleCharacterPhaseCoordinator::HandlePhaseUIFinished);
    return true;
}

void UBattleCharacterPhaseCoordinator::Shutdown()
{
    if (BattleManager)
    {
        BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
    }

    CancelCharacterPhaseExecution();
    StepRunner = nullptr;
    BattleManager = nullptr;
}

void UBattleCharacterPhaseCoordinator::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

void UBattleCharacterPhaseCoordinator::HandlePhaseUIFinished(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    if (!RequiresSequentialExecution(NewPhase))
    {
        return;
    }

    BeginCharacterPhaseExecution(OldPhase, NewPhase);
}

bool UBattleCharacterPhaseCoordinator::RequiresSequentialExecution(EBattlePhase Phase) const
{
    switch (Phase)
    {
    case EBattlePhase::BattleStart:
    case EBattlePhase::RoundStart:
    case EBattlePhase::ExchangeStart:
    case EBattlePhase::ExchangeEnd:
    case EBattlePhase::BattleActionSequenceStart:
    case EBattlePhase::BattleActionSequenceEnd:
    case EBattlePhase::RoundEnd:
    case EBattlePhase::BattleEnd:
        return true;

    default:
        return false;
    }
}

bool UBattleCharacterPhaseCoordinator::RequiresManualCompletion(EBattlePhase Phase) const
{
    return Phase == EBattlePhase::RoundStart || Phase == EBattlePhase::RoundEnd;
}

void UBattleCharacterPhaseCoordinator::BeginCharacterPhaseExecution(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    if (!StepRunner)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacterPhaseCoordinator] StepRunner is not initialized."));
        FinishCharacterPhaseExecution(NewPhase);
        return;
    }

    if (StepRunner->IsRunning())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BattleCharacterPhaseCoordinator] Interrupted character phase execution. Previous=%d, New=%d"), static_cast<int32>(ExecutingPhase), static_cast<int32>(NewPhase));
        CancelCharacterPhaseExecution();
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager ? BattleManager->GetBattleRuntimeContext() : nullptr;

    if (!RuntimeContext)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacterPhaseCoordinator] BattleRuntimeContext is not available."));
        FinishCharacterPhaseExecution(NewPhase);
        return;
    }

    const TWeakObjectPtr<ABattleCharacterBase> PlayerCharacter = RuntimeContext->GetPlayerCharacter();
    const TWeakObjectPtr<ABattleCharacterBase> EnemyCharacter = RuntimeContext->GetEnemyCharacter();
    TArray<FBattleAsyncStep> Steps;
    Steps.Reserve(4);
    Steps.Add([this, PlayerCharacter, OldPhase, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecutePassiveStep(PlayerCharacter.Get(), OldPhase, NewPhase, CompletionDelegate);
    });

    Steps.Add([this, EnemyCharacter, OldPhase, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecutePassiveStep(EnemyCharacter.Get(), OldPhase, NewPhase, CompletionDelegate);
    });

    Steps.Add([this, PlayerCharacter, OldPhase, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecuteStatusEffectStep(PlayerCharacter.Get(), OldPhase, NewPhase, CompletionDelegate);
    });

    Steps.Add([this, EnemyCharacter, OldPhase, NewPhase](const FSimpleDelegate& CompletionDelegate)
    {
        ExecuteStatusEffectStep(EnemyCharacter.Get(), OldPhase, NewPhase, CompletionDelegate);
    });

    ExecutingPhase = NewPhase;
    StepRunner->Start(MoveTemp(Steps), FSimpleDelegate::CreateUObject(this, &UBattleCharacterPhaseCoordinator::FinishCharacterPhaseExecution, NewPhase));
}

void UBattleCharacterPhaseCoordinator::ExecutePassiveStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const
{
    if (Character && Character->GetPassiveComponent())
    {
        if (RequiresManualCompletion(NewPhase))
        {
            Character->GetPassiveComponent()->ExecuteRoundPhaseSequentially(NewPhase, CompletionDelegate);
            return;
        }

        Character->GetPassiveComponent()->NotifyBattlePhaseChanged(OldPhase, NewPhase);
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    CompletionDelegate.ExecuteIfBound();
}

void UBattleCharacterPhaseCoordinator::ExecuteStatusEffectStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const
{
    if (Character && Character->GetStatusEffectComponent())
    {
        if (RequiresManualCompletion(NewPhase))
        {
            Character->GetStatusEffectComponent()->ExecuteRoundPhaseSequentially(NewPhase, CompletionDelegate);
            return;
        }

        Character->GetStatusEffectComponent()->NotifyBattlePhaseChanged(OldPhase, NewPhase);
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    CompletionDelegate.ExecuteIfBound();
}

void UBattleCharacterPhaseCoordinator::FinishCharacterPhaseExecution(EBattlePhase FinishedPhase)
{
    if (ExecutingPhase == FinishedPhase) ExecutingPhase = EBattlePhase::None;

    if (!BattleManager || BattleManager->GetCurrentPhase() != FinishedPhase)
    {
        return;
    }

    BattleManager->GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UBattleCharacterPhaseCoordinator::NotifyCharacterPhaseFinishedIfCurrent, FinishedPhase));
}

void UBattleCharacterPhaseCoordinator::NotifyCharacterPhaseFinishedIfCurrent(EBattlePhase FinishedPhase)
{
    if (!BattleManager || BattleManager->GetCurrentPhase() != FinishedPhase)
    {
        return;
    }

    BattleManager->NotifyCharacterPhaseExecutionFinished(FinishedPhase);
}

void UBattleCharacterPhaseCoordinator::CancelCharacterPhaseExecution()
{
    if (StepRunner)
    {
        StepRunner->Cancel();
    }

    ExecutingPhase = EBattlePhase::None;
}
