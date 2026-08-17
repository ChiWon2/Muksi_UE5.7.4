#include "Muksi/Contents/Battle/Flow/BattleCharacterPhaseCoordinator.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Flow/BattleAsyncStepRunner.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"

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

    BattleManager->PhasePrepRequestedDelegate.AddUniqueDynamic(this, &UBattleCharacterPhaseCoordinator::HandlePhasePrepRequested);

    return true;
}

void UBattleCharacterPhaseCoordinator::Shutdown()
{
    if (BattleManager)
    {
        BattleManager->PhasePrepRequestedDelegate.RemoveDynamic(this, &UBattleCharacterPhaseCoordinator::HandlePhasePrepRequested);
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

void UBattleCharacterPhaseCoordinator::HandlePhasePrepRequested(EBattlePhase OldPhase,EBattlePhase NewPhase,UBattlePhaseTaskContext* TaskContext)
{
    if (!HandlesPhasePrep(NewPhase) || !TaskContext)
    {
        return;
    }

    if (StepRunner && StepRunner->IsRunning())
    {
        CancelCharacterPhaseExecution();
    }

    PhasePrepTask = TaskContext->RegisterTask(this);

    if (!PhasePrepTask)
    {
        return;
    }

    BeginCharacterPhaseExecution(OldPhase, NewPhase);
}

bool UBattleCharacterPhaseCoordinator::HandlesPhasePrep(EBattlePhase Phase) const
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

void UBattleCharacterPhaseCoordinator::BeginCharacterPhaseExecution(EBattlePhase OldPhase,EBattlePhase NewPhase)
{
    if (!StepRunner)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacterPhaseCoordinator] StepRunner is not initialized."));
        FinishCharacterPhaseExecution(NewPhase);
        return;
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

    const bool bStarted = StepRunner->Start(MoveTemp(Steps), FSimpleDelegate::CreateUObject(this,&UBattleCharacterPhaseCoordinator::FinishCharacterPhaseExecution,NewPhase));

    if (!bStarted)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleCharacterPhaseCoordinator] Failed to start character phase execution."));
        FinishCharacterPhaseExecution(NewPhase);
    }
}

void UBattleCharacterPhaseCoordinator::ExecutePassiveStep( ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const
{
    UCharacterPassiveComponent* PassiveComponent = Character ? Character->GetPassiveComponent() : nullptr;

    if (!PassiveComponent)
    {
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    PassiveComponent->ExecuteSequentially(OldPhase, NewPhase, CompletionDelegate);
}

void UBattleCharacterPhaseCoordinator::ExecuteStatusEffectStep(ABattleCharacterBase* Character,EBattlePhase OldPhase,EBattlePhase NewPhase,const FSimpleDelegate& CompletionDelegate) const
{
    UMuksiStatusEffectComponent* StatusEffectComponent = Character ? Character->GetStatusEffectComponent() : nullptr;

    if (!StatusEffectComponent)
    {
        CompletionDelegate.ExecuteIfBound();
        return;
    }

    StatusEffectComponent->ExecuteSequentially(OldPhase, NewPhase, CompletionDelegate);
}

void UBattleCharacterPhaseCoordinator::FinishCharacterPhaseExecution(EBattlePhase FinishedPhase)
{
    if (!BattleManager || BattleManager->GetCurrentPhase() != FinishedPhase)
    {
        PhasePrepTask = nullptr;
        return;
    }

    UBattlePhaseTask* CompletedTask = PhasePrepTask;
    PhasePrepTask = nullptr;

    if (CompletedTask)
    {
        CompletedTask->Complete();
    }
}

void UBattleCharacterPhaseCoordinator::CancelCharacterPhaseExecution()
{
    if (StepRunner)
    {
        StepRunner->Cancel();
    }
    PhasePrepTask = nullptr;
}
