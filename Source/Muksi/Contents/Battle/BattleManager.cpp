#include "Muksi/Contents/Battle/BattleManager.h"

#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"
#include "Muksi/Contents/Battle/Flow/BattlePhasePipeline.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Setup/BattleSetupManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Save/BattleEncounterSubsystem.h"

ABattleManager::ABattleManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleManager::BeginPlay()
{
    Super::BeginPlay();
    BattleRuntimeContext = NewObject<UBattleRuntimeContext>(this);

    if (BattleRuntimeContext) 
        BattleRuntimeContext->ResetBattle();

    PhasePipeline = NewObject<UBattlePhasePipeline>(this);
    if (!BattleRuntimeContext || !PhasePipeline || !PhasePipeline->Initialize(this))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to initialize battle phase pipeline."));
        PhasePipeline = nullptr;
    }

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this)) 
        ManagerSubsystem->RegisterManager<ABattleManager>(this);
}

void ABattleManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (PhasePipeline) 
        PhasePipeline->Shutdown();

    PhasePipeline = nullptr;
    BattleRuntimeContext = nullptr;
    bBattleFlowInitialized = false;
    bBattleFlowStarted = false;

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this)) 
        ManagerSubsystem->UnregisterManager<ABattleManager>(this);

    Super::EndPlay(EndPlayReason);
}

bool ABattleManager::InitializeBattleFlow()
{
    if (bBattleFlowInitialized)
        return true;

    if (!IsValid(BattleRuntimeContext) || !IsValid(PhasePipeline))
        return false;

    if (!IsValid(BattleSetupManager) || !IsValid(BattleGridManager) || !IsValid(BattleTargetingManager) || !IsValid(BattleSimulationManager) || !IsValid(BattleSequenceManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Required manager reference is missing. Setup=%s Grid=%s Targeting=%s Simulation=%s Sequence=%s"), *GetNameSafe(BattleSetupManager), *GetNameSafe(BattleGridManager), *GetNameSafe(BattleTargetingManager), *GetNameSafe(BattleSimulationManager), *GetNameSafe(BattleSequenceManager));
        return false;
    }

    if (!BattleSetupManager->InitializeBattleFlow(this, BattleGridManager))
        return false;

    if (!BattleSimulationManager->InitializeBattleFlow(this, BattleRuntimeContext, BattleGridManager))
        return false;

    if (!BattleTargetingManager->InitializeBattleFlow(this, BattleRuntimeContext, BattleGridManager, BattleSimulationManager))
        return false;

    if (!BattleSequenceManager->InitializeBattleFlow(this, BattleRuntimeContext, BattleGridManager))
        return false;

    bBattleFlowInitialized = true;
    return true;
}

void ABattleManager::StartBattleFlow()
{
    if (bBattleFlowStarted)
        return;

    if (!InitializeBattleFlow())
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Failed to initialize battle flow."));
        return;
    }

    bBattleFlowStarted = true;
    ReadyStart();
}

void ABattleManager::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
    BattleActionStartDelegate.Broadcast(BattleAction);
}

void ABattleManager::ReadyStart()
{
    bIsCharacterDead = false;
    ChangePhase(EBattlePhase::ReadyStart);
}

void ABattleManager::ReadyEnd()
{
    ChangePhase(EBattlePhase::ReadyEnd);
}

void ABattleManager::BattleStart()
{
    CurrentRound = 0;
    CurrentExchange = 0;
    if (BattleRuntimeContext) BattleRuntimeContext->ResetRound(CurrentRound);
    ChangePhase(EBattlePhase::BattleStart);
}

void ABattleManager::BattleEnd()
{
    if (CurrentPhase == EBattlePhase::BattleEnd) return;
    ChangePhase(EBattlePhase::BattleEnd);
}

void ABattleManager::RoundStart()
{
    ++CurrentRound;
    CurrentExchange = 0;
    if (BattleRuntimeContext) BattleRuntimeContext->ResetRound(CurrentRound);
    ChangePhase(EBattlePhase::RoundStart);
}

void ABattleManager::RoundEnd()
{
    ChangePhase(EBattlePhase::RoundEnd);
}

void ABattleManager::ExchangeStart()
{
    if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid exchange start index: %d"), CurrentExchange);
        return;
    }
    if (BattleRuntimeContext) BattleRuntimeContext->SetCurrentExchange(CurrentExchange);
    ChangePhase(EBattlePhase::ExchangeStart);
}

void ABattleManager::StartExchangeSelectCard()
{
    if (CurrentExchange < 0 || CurrentExchange >= MaxExchangeCount)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Invalid current exchange: %d"), CurrentExchange);
        return;
    }
    ChangePhase(EBattlePhase::CardSelect);
}

void ABattleManager::ExchangeEnd()
{
    ChangePhase(EBattlePhase::ExchangeEnd);
}

void ABattleManager::BattleActionSequenceStart()
{
    if (!BattleRuntimeContext || !BattleRuntimeContext->HasBattleActionSequenceActions())
    {
        UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Battle Action Sequence에 실행할 행동이 없습니다."));
        BattleActionSequenceEnd();
        return;
    }
    ChangePhase(EBattlePhase::BattleActionSequenceStart);
}

void ABattleManager::BattleActionSequenceEnd()
{
    if (CurrentPhase == EBattlePhase::BattleActionSequenceEnd) return;
    ChangePhase(EBattlePhase::BattleActionSequenceEnd);
}

void ABattleManager::ChangePhase(EBattlePhase NewPhase)
{
    if (CurrentPhase == NewPhase)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] You tried to change to the same phase."));
        return;
    }

    if (!PhasePipeline)
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleManager] Phase pipeline is unavailable."));
        return;
    }

    PreviousPhase = CurrentPhase;
    CurrentPhase = NewPhase;

    ExecutePhaseEntry();
}

void ABattleManager::ExecutePhaseEntry()
{
    PhasePipeline->ExecutePhaseEntry(PreviousPhase, CurrentPhase, FSimpleDelegate::CreateUObject(this, &ABattleManager::HandlePhaseEntryFinished, CurrentPhase));
}

void ABattleManager::HandlePhaseEntryFinished(EBattlePhase FinishedPhase)
{
    if (!IsCurrentPhaseCompletion(FinishedPhase, TEXT("Entry")))  
        return;
    ExecutePhaseUI();
}

void ABattleManager::ExecutePhaseUI()
{
    PhasePipeline->ExecutePhaseUI(PreviousPhase, CurrentPhase, FSimpleDelegate::CreateUObject(this, &ABattleManager::HandlePhaseUIFinished, CurrentPhase));
}

void ABattleManager::HandlePhaseUIFinished(EBattlePhase FinishedPhase)
{
    if (!IsCurrentPhaseCompletion(FinishedPhase, TEXT("UI"))) 
        return;
    ExecutePhasePrep();
}

void ABattleManager::ExecutePhasePrep()
{
    PhasePipeline->ExecutePhasePrep(PreviousPhase, CurrentPhase, FSimpleDelegate::CreateUObject(this, &ABattleManager::HandlePhasePrepFinished, CurrentPhase));
}

void ABattleManager::HandlePhasePrepFinished(EBattlePhase FinishedPhase)
{
    if (!IsCurrentPhaseCompletion(FinishedPhase, TEXT("Prep")))
        return;
    ExecutePhaseExecution();
}

void ABattleManager::ExecutePhaseExecution()
{
    PhasePipeline->ExecutePhaseExecution(PreviousPhase, CurrentPhase, FSimpleDelegate::CreateUObject(this, &ABattleManager::HandlePhaseExecutionFinished, CurrentPhase));
}

void ABattleManager::HandlePhaseExecutionFinished(EBattlePhase FinishedPhase)
{
    if (!IsCurrentPhaseCompletion(FinishedPhase, TEXT("Execution")))
        return;
    AdvanceFromPhase(FinishedPhase);
}

bool ABattleManager::IsCurrentPhaseCompletion(EBattlePhase FinishedPhase, const TCHAR* StageName) const
{
    if (CurrentPhase == FinishedPhase) 
        return true;
    UE_LOG(LogTemp, Warning, TEXT("[BattleManager] Ignored stale %s completion. Current=%d Finished=%d"), StageName, static_cast<int32>(CurrentPhase), static_cast<int32>(FinishedPhase));
    return false;
}

void ABattleManager::NotifyBattleCharacterDead()
{
    if (bIsCharacterDead) return;
    bIsCharacterDead = true;
    BattleEnd();
}

void ABattleManager::EndBattleLevel()
{
    UBattleEncounterSubsystem* EncounterSubsystem = UBattleEncounterSubsystem::Get(this);
    if (!IsValid(EncounterSubsystem)) 
        return;
    FBattleResult BattleResult;
    EncounterSubsystem->FinishBattleEncounter(BattleResult);
}

bool ABattleManager::ShouldEndBattle() const
{
    return bIsCharacterDead;
}

void ABattleManager::AdvanceExchange()
{
    if (CurrentExchange + 1 < MaxExchangeCount)
    {
        ++CurrentExchange;
        ExchangeStart();
        return;
    }
    BattleActionSequenceStart();
}

void ABattleManager::RestartCurrentExchangeCardSelection()
{
    if (CurrentPhase != EBattlePhase::Targeting && CurrentPhase != EBattlePhase::CardReveal) return;
    ChangePhase(EBattlePhase::CardSelect);
}

void ABattleManager::AdvanceFromPhase(EBattlePhase FinishedPhase)
{
    switch (FinishedPhase)
    {
    case EBattlePhase::ReadyStart:
        ReadyEnd();
        break;
    case EBattlePhase::ReadyEnd:
        BattleStart();
        break;
    case EBattlePhase::BattleStart:
        RoundStart();
        break;
    case EBattlePhase::RoundStart:
        ExchangeStart();
        break;
    case EBattlePhase::ExchangeStart:
        StartExchangeSelectCard();
        break;
    case EBattlePhase::CardSelect:
        ChangePhase(EBattlePhase::Targeting);
        break;
    case EBattlePhase::Targeting:
        ChangePhase(EBattlePhase::CardReveal);
        break;
    case EBattlePhase::CardReveal:
        ChangePhase(EBattlePhase::SimulationSequence);
        break;
    case EBattlePhase::SimulationSequence:
        ExchangeEnd();
        break;
    case EBattlePhase::ExchangeEnd:
        AdvanceExchange();
        break;
    case EBattlePhase::BattleActionSequenceStart:
        BattleActionSequenceEnd();
        break;
    case EBattlePhase::BattleActionSequenceEnd:
        RoundEnd();
        break;
    case EBattlePhase::RoundEnd:
        if (ShouldEndBattle()) 
            BattleEnd();
        else 
            RoundStart();
        break;
    case EBattlePhase::BattleEnd:
        EndBattleLevel();
        break;
    default:
        break;
    }
}
