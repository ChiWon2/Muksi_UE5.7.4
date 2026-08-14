#include "Muksi/Contents/Battle/Flow/BattlePhasePipeline.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Flow/BattleCharacterPhaseCoordinator.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"

bool UBattlePhasePipeline::Initialize(ABattleManager* InBattleManager)
{
    Shutdown();
    if (!IsValid(InBattleManager)) return false;
    BattleManager = InBattleManager;
    CharacterPhaseCoordinator = NewObject<UBattleCharacterPhaseCoordinator>(this);
    if (!CharacterPhaseCoordinator || !CharacterPhaseCoordinator->Initialize(BattleManager))
    {
        Shutdown();
        return false;
    }
    return true;
}

void UBattlePhasePipeline::Shutdown()
{
    CancelActiveStage();
    if (CharacterPhaseCoordinator) CharacterPhaseCoordinator->Shutdown();
    CharacterPhaseCoordinator = nullptr;
    BattleManager = nullptr;
}

void UBattlePhasePipeline::BeginDestroy()
{
    Shutdown();
    Super::BeginDestroy();
}

void UBattlePhasePipeline::ExecutePhaseEntry(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate)
{
    UBattlePhaseTaskContext* StageContext = BeginStage(NewPhase, MoveTemp(CompletionDelegate));

    if (BattleManager && StageContext) 
        BattleManager->PhaseEntryRequestedDelegate.Broadcast(OldPhase, NewPhase, StageContext);

    SealStage(StageContext);
}

void UBattlePhasePipeline::ExecutePhaseUI(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate)
{
    UBattlePhaseTaskContext* StageContext = BeginStage(NewPhase, MoveTemp(CompletionDelegate));

    if (BattleManager && StageContext) 
        BattleManager->PhaseUIRequestedDelegate.Broadcast(OldPhase, NewPhase, StageContext);
    
    SealStage(StageContext);
}

void UBattlePhasePipeline::ExecutePhasePrep(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate)
{
    UBattlePhaseTaskContext* StageContext = BeginStage(NewPhase, MoveTemp(CompletionDelegate));
    
    if (BattleManager && StageContext) 
        BattleManager->PhasePrepRequestedDelegate.Broadcast(OldPhase, NewPhase, StageContext);
    
    SealStage(StageContext);
}

void UBattlePhasePipeline::ExecutePhaseExecution(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate)
{
    UBattlePhaseTaskContext* StageContext = BeginStage(NewPhase, MoveTemp(CompletionDelegate));
    
    if (BattleManager && StageContext) 
        BattleManager->PhaseExecutionRequestedDelegate.Broadcast(OldPhase, NewPhase, StageContext);
    
    SealStage(StageContext);
}

UBattlePhaseTaskContext* UBattlePhasePipeline::BeginStage(EBattlePhase Phase, FSimpleDelegate CompletionDelegate)
{
    CancelActiveStage();
    ++StageExecutionSerial;
    ActiveStageCompletionDelegate = MoveTemp(CompletionDelegate);
    ActiveTaskContext = NewObject<UBattlePhaseTaskContext>(this);
    if (!ActiveTaskContext)
    {
        FSimpleDelegate FailedStageCompletionDelegate = MoveTemp(ActiveStageCompletionDelegate);
        ActiveStageCompletionDelegate.Unbind();
        FailedStageCompletionDelegate.ExecuteIfBound();
        return nullptr;
    }
    ActiveTaskContext->Initialize(this, Phase, StageExecutionSerial);
    return ActiveTaskContext;
}

void UBattlePhasePipeline::SealStage(UBattlePhaseTaskContext* StageContext)
{
    if (!StageContext || ActiveTaskContext != StageContext) 
        return;
    StageContext->SealRegistration();
}

void UBattlePhasePipeline::HandleTaskContextFinished(UBattlePhaseTaskContext* FinishedContext, int32 FinishedSerial)
{
    if (!BattleManager || ActiveTaskContext != FinishedContext || StageExecutionSerial != FinishedSerial) 
        return;
    if (BattleManager->GetCurrentPhase() != FinishedContext->GetPhase()) 
        return;
    ActiveTaskContext = nullptr;
    FSimpleDelegate CompletionDelegate = MoveTemp(ActiveStageCompletionDelegate);
    ActiveStageCompletionDelegate.Unbind();
    CompletionDelegate.ExecuteIfBound();
}

void UBattlePhasePipeline::CancelActiveStage()
{
    if (ActiveTaskContext) 
        ActiveTaskContext->Cancel();

    ActiveTaskContext = nullptr;
    ActiveStageCompletionDelegate.Unbind();
}
