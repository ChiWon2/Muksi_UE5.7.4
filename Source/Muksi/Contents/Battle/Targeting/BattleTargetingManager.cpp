#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Grid/Tiles/BattleGridTile.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"

ABattleTargetingManager::ABattleTargetingManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleTargetingManager::BeginPlay()
{
    Super::BeginPlay();
}

void ABattleTargetingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);

    ClearSelectionAndRevealPreviews();
    if (BattleManager)
        BattleManager->PhaseEntryRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseEntryRequested);

    if (BattleManager)
        BattleManager->PhaseUIRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseUIRequested);

    if (BattleManager)
        BattleManager->PhaseExecutionRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseExecutionRequested);

    PhaseUITask = nullptr;
    PhaseExecutionTask = nullptr;
    PendingPlayerCard = nullptr;
    BattleRuntimeContext = nullptr;
    BattleGridManager = nullptr;
    BattleSimulationManager = nullptr;
    BattleManager = nullptr;

    Super::EndPlay(EndPlayReason);
}

bool ABattleTargetingManager::InitializeBattleFlow(ABattleManager* InBattleManager, UBattleRuntimeContext* InBattleRuntimeContext, ABattleGridManager* InBattleGridManager, ABattleSimulationManager* InBattleSimulationManager)
{
    if (!IsValid(InBattleManager) || !IsValid(InBattleRuntimeContext) || !IsValid(InBattleGridManager) || !IsValid(InBattleSimulationManager))
        return false;

    BattleManager = InBattleManager;
    BattleRuntimeContext = InBattleRuntimeContext;
    BattleGridManager = InBattleGridManager;
    BattleSimulationManager = InBattleSimulationManager;
    BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseEntryRequested);
    BattleManager->PhaseUIRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseUIRequested);
    BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseExecutionRequested);

    if (!TargetingPresentationController)
        TargetingPresentationController = NewObject<UTargetingPresentationController>(this);

    if (!TargetingPresentationController)
        return false;

    TargetingPresentationController->Initialize(BattleGridManager);

    return true;
}

void ABattleTargetingManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    if (!ShouldHandlePhaseEntry(NewPhase) || !TaskContext) 
        return;
    UBattlePhaseTask* Task = TaskContext->RegisterTask(this);
    if (!Task) 
        return;
    if (!IsValid(BattleManager) || !IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager) || !IsValid(BattleSimulationManager))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleTargetingManager] Failed to resolve phase entry dependencies."));
        Task->Complete();
        return;
    }

    switch (NewPhase)
    {
    case EBattlePhase::RoundStart:
        ResetCurrentExchangeTargeting();
        break;

    case EBattlePhase::CardSelect:
        if (OldPhase == EBattlePhase::Targeting) 
            ResetPlayerTargetingForCardReselection();
        else 
            ResetCurrentExchangeTargeting();
        break;

    case EBattlePhase::Targeting:
        if (!StartPendingPlayerTargeting())
        {
            UE_LOG(LogTemp, Error, TEXT("[BattleTargetingManager] Failed to start player targeting."));
            GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::Targeting)
                {
                    BattleManager->RestartCurrentExchangeCardSelection();
                }
            }));
        }
        break;

    case EBattlePhase::SimulationSequence:
    case EBattlePhase::ExchangeEnd:
    case EBattlePhase::BattleActionSequenceStart:
    case EBattlePhase::RoundEnd:
    case EBattlePhase::BattleEnd:
        ClearSelectionAndRevealPreviews();
        PendingPlayerCard = nullptr;
        break;

    default:
        break;
    }

    Task->Complete();
}

bool ABattleTargetingManager::ShouldHandlePhaseEntry(EBattlePhase Phase) const
{
    switch (Phase)
    {
    case EBattlePhase::RoundStart:
    case EBattlePhase::CardSelect:
    case EBattlePhase::Targeting:
    case EBattlePhase::SimulationSequence:
    case EBattlePhase::ExchangeEnd:
    case EBattlePhase::BattleActionSequenceStart:
    case EBattlePhase::RoundEnd:
    case EBattlePhase::BattleEnd:
        return true;
    default:
        return false;
    }
}

void ABattleTargetingManager::HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)OldPhase;
    if (NewPhase != EBattlePhase::CardReveal || !TaskContext) return;
    PhaseUITask = TaskContext->RegisterTask(this);
}

void ABattleTargetingManager::HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)OldPhase;
    if ((NewPhase != EBattlePhase::CardSelect && NewPhase != EBattlePhase::Targeting) || !TaskContext) return;
    PhaseExecutionTask = TaskContext->RegisterTask(this);
}

void ABattleTargetingManager::ResetCurrentExchangeTargeting()
{
    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
    
    ClearSelectionAndRevealPreviews();
    PendingPlayerCard = nullptr;
    bPlayerCardSelectionFinished = false;
    bEnemyCardSelectionFinished = false;

    if (BattleRuntimeContext && BattleManager)
    {
        BattleRuntimeContext->ResetExchangeActions(BattleManager->GetCurrentExchange());
    }
}

void ABattleTargetingManager::ResetPlayerTargetingForCardReselection()
{
    GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);

    if (TargetingPresentationController) TargetingPresentationController->ClearSession(PlayerTargetingSession);
    else if (PlayerTargetingSession)
    {
        PlayerTargetingSession->EndSession();
        PlayerTargetingSession = nullptr;
    }

    PendingPlayerCard = nullptr;
    bPlayerCardSelectionFinished = false;

    if (BattleRuntimeContext && BattleManager) BattleRuntimeContext->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());
}

void ABattleTargetingManager::ClearSelectionAndRevealPreviews()
{
    if (TargetingPresentationController)
    {
        TargetingPresentationController->ClearAll(PlayerTargetingSession, EnemyTargetingSession);
        return;
    }

    if (PlayerTargetingSession)
    {
        PlayerTargetingSession->EndSession();
        PlayerTargetingSession = nullptr;
    }

    if (EnemyTargetingSession)
    {
        EnemyTargetingSession->EndSession();
        EnemyTargetingSession = nullptr;
    }

    if (BattleGridManager)
    {
        BattleGridManager->AllClearGridHovered();
        BattleGridManager->AllClearExchangeIndicator();
    }
}

bool ABattleTargetingManager::RequestPlayerCardSelection(UMuksiBattleCardDataAsset* CardData)
{
    if (!IsValid(BattleManager) || !IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager))
    {
        return false;
    }

    if (!IsValid(CardData) || BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect)
    {
        return false;
    }

    PendingPlayerCard = CardData;
    UBattlePhaseTask* CompletedTask = PhaseExecutionTask;
    PhaseExecutionTask = nullptr;
    if (CompletedTask) CompletedTask->Complete();

    // Phase delegate broadcast가 끝난 뒤 즉시 완료 카드를 처리해 재진입 전환을 피한다.
    if (PlayerTargetingSession && PlayerTargetingSession->IsCompleted())
    {
        NotifyPlayerCardSelectionFinished();
    }

    return IsPlayerCardTargeting() || bPlayerCardSelectionFinished;
}

bool ABattleTargetingManager::StartPendingPlayerTargeting()
{
    ABattleGridManager* RuntimeGridManager = ResolveRuntimeGridManager();
    if (!IsValid(PendingPlayerCard) || !IsValid(RuntimeGridManager))
    {
        return false;
    }

    if (TargetingPresentationController)
    {
        TargetingPresentationController->ClearExecutionPreview();
        TargetingPresentationController->ClearSession(PlayerTargetingSession);
    }
    else if (PlayerTargetingSession)
    {
        PlayerTargetingSession->EndSession();
        PlayerTargetingSession = nullptr;
    }

    ABattleCharacterBase* PlayerTargetingActor = GetPlayerTargetingActor();
    if (!PlayerTargetingActor)
    {
        return false;
    }

    PlayerTargetingSession = NewObject<UBattleTargetingSession>(this);
    if (!PlayerTargetingSession || !PlayerTargetingSession->StartSession(
        PlayerTargetingActor,
        RuntimeGridManager,
        ResolveRuntimeGridWorldType(),
        PendingPlayerCard->TargetingData,
        true))
    {
        PlayerTargetingSession = nullptr;
        return false;
    }

    return true;
}

bool ABattleTargetingManager::RequestConfirmPlayerTargeting()
{
    if (!HasActivePlayerTargetingSession())
    {
        return false;
    }

    ConfirmPlayerCardTargetingStep();
    return true;
}

bool ABattleTargetingManager::RequestUndoOrCancelPlayerTargeting()
{
    if (!HasActivePlayerTargetingSession())
    {
        return false;
    }

    if (!UndoPlayerCardTargetingStep())
    {
        CancelPlayerCardTargeting();
    }

    return true;
}

void ABattleTargetingManager::RequestCancelPlayerTargeting()
{
    CancelPlayerCardTargeting();
}

bool ABattleTargetingManager::RequestUpdatePlayerTargeting(const FHitResult& HitResult, bool bHasHitResult)
{
    if (!IsPlayerCardTargeting())
    {
        return false;
    }

    if (!bHasHitResult)
    {
        UpdatePlayerTargetingAim(FVector::ZeroVector, false);
        UpdatePlayerTargetingCandidate(FHexOffsetCoord::Invalid());
        return true;
    }

    UpdatePlayerTargetingAim(HitResult.ImpactPoint, true);

    FHexOffsetCoord CandidateCoord = FHexOffsetCoord::Invalid();
    ResolveGridCoordFromHit(HitResult, CandidateCoord);
    UpdatePlayerTargetingCandidate(CandidateCoord);
    return true;
}

bool ABattleTargetingManager::UpdatePlayerTargetingCandidate(const FHexOffsetCoord& CandidateCoord)
{
    return PlayerTargetingSession && PlayerTargetingSession->UpdateCandidateCoord(CandidateCoord);
}

void ABattleTargetingManager::UpdatePlayerTargetingAim(const FVector& AimWorldLocation, bool bHasAimLocation)
{
    if (PlayerTargetingSession)
    {
        PlayerTargetingSession->UpdateAimWorldLocation(AimWorldLocation, bHasAimLocation);
    }
}

ETargetingConfirmResult ABattleTargetingManager::ConfirmPlayerCardTargetingStep()
{
    if (!PlayerTargetingSession)
    {
        return ETargetingConfirmResult::Failed;
    }

    const ETargetingConfirmResult ConfirmResult = PlayerTargetingSession->ConfirmStep();
    if (ConfirmResult == ETargetingConfirmResult::Completed)
    {
        NotifyPlayerCardSelectionFinished();
    }

    return ConfirmResult;
}

bool ABattleTargetingManager::UndoPlayerCardTargetingStep()
{
    if (!PlayerTargetingSession || !BattleManager || BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    if (!PlayerTargetingSession->UndoStep())
    {
        return false;
    }

    bPlayerCardSelectionFinished = false;
    if (BattleRuntimeContext)
    {
        BattleRuntimeContext->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());
    }

    UE_LOG(LogTemp, Log, TEXT("[BattleTargetingManager] Player targeting undo. RestoredStep=%d"),
        PlayerTargetingSession->GetCurrentStepIndex());
    return true;
}

void ABattleTargetingManager::CancelPlayerCardTargeting()
{
    if (TargetingPresentationController)
    {
        TargetingPresentationController->ClearSession(PlayerTargetingSession);
    }
    else if (PlayerTargetingSession)
    {
        PlayerTargetingSession->EndSession();
        PlayerTargetingSession = nullptr;
    }

    PendingPlayerCard = nullptr;
    bPlayerCardSelectionFinished = false;

    if (BattleRuntimeContext && BattleManager)
    {
        BattleRuntimeContext->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());
    }

    if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::Targeting)
    {
        BattleManager->RestartCurrentExchangeCardSelection();
    }
}

bool ABattleTargetingManager::IsPlayerCardTargeting() const
{
    return PlayerTargetingSession
        && BattleManager
        && BattleManager->GetCurrentPhase() == EBattlePhase::Targeting
        && (PlayerTargetingSession->IsSelecting() || PlayerTargetingSession->IsCompleted());
}

bool ABattleTargetingManager::HasActivePlayerTargetingSession() const
{
    return PlayerTargetingSession
        && BattleManager
        && BattleManager->GetCurrentPhase() == EBattlePhase::Targeting;
}

void ABattleTargetingManager::NotifyPlayerCardSelectionFinished()
{
    if (!BuildPlayerActionForCurrentExchange())
    {
        return;
    }

    bPlayerCardSelectionFinished = true;
    TryFinishCurrentExchangeTargeting();
}

bool ABattleTargetingManager::BuildPlayerActionForCurrentExchange()
{
    if (!BattleRuntimeContext || !BattleManager || !PlayerTargetingSession
        || !PlayerTargetingSession->IsCompleted() || !IsValid(PendingPlayerCard))
    {
        return false;
    }

    ABattleCharacterBase* PlayerCharacter = BattleRuntimeContext->GetPlayerCharacter();
    if (!IsValid(PlayerCharacter))
    {
        return false;
    }

    FBattleAction BattleAction;
    BattleAction.ExchangeIndex = BattleManager->GetCurrentExchange();
    BattleAction.Card = PendingPlayerCard;
    BattleAction.Speed = PlayerCharacter->GetCharacterSpeed() + PendingPlayerCard->CardSpeed;
    BattleAction.Attacker = PlayerCharacter;
    BattleAction.bPlayerAction = true;
    BattleAction.TargetingIntent = PlayerTargetingSession->GetIntent();

    BattleRuntimeContext->SetPlayerExchangeAction(BattleAction.ExchangeIndex, BattleAction);
    return true;
}

bool ABattleTargetingManager::RequestEnemyCardSelection()
{
    if (!IsValid(BattleManager) || !IsValid(BattleRuntimeContext))
    {
        return false;
    }

    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    if (BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex)) return true;
    if (GetWorldTimerManager().IsTimerActive(EnemyCardSelectionTimerHandle)) return true;

    ABattleCharacter_Enemy* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();
    if (!IsValid(EnemyCharacter))
    {
        return false;
    }

    const FVector2D SelectSpeed = EnemyCharacter->GetCurrentSelectCardTime();
    const float RandomDelay = FMath::FRandRange(SelectSpeed.X, SelectSpeed.Y);

    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().SetTimer(
        EnemyCardSelectionTimerHandle,
        this,
        &ABattleTargetingManager::CompleteEnemyCardSelectionRequest,
        RandomDelay,
        false);

    return true;
}

void ABattleTargetingManager::CompleteEnemyCardSelectionRequest()
{
    if (!BuildEnemyActionForCurrentExchange() || !BattleRuntimeContext || !BattleManager)
    {
        return;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
    if (!EnemyAction || !IsValid(EnemyAction->Card))
    {
        return;
    }

    UMuksiBattleCardDataAsset* PresentedCard = EnemyAction->Card.Get();
    UMuksiBattleCardDataAsset* DeceivedCard = EnemyAction->Card->GetDeceivedCard();
    if (IsValid(DeceivedCard))
    {
        PresentedCard = DeceivedCard;
    }

    OnEnemyCardSelectionReady.Broadcast(PresentedCard, ExchangeIndex);
}

bool ABattleTargetingManager::BuildEnemyActionForCurrentExchange()
{
    if (!IsValid(BattleManager) || !IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager))
    {
        return false;
    }

    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    ABattleGridManager* RuntimeGridManager = ResolveRuntimeGridManager();
    ABattleCharacter_Enemy* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();
    ABattleCharacterBase* EnemyTargetingActor = GetEnemyTargetingActor();
    ABattleCharacterBase* PlayerTargetingActor = GetPlayerTargetingActor();
    if (!IsValid(RuntimeGridManager) || !IsValid(EnemyCharacter) || !EnemyTargetingActor || !PlayerTargetingActor)
    {
        return false;
    }

    UMuksiBattleCardDataAsset* SelectedCard = EnemyCharacter->SelectCardForExchange(
        RuntimeGridManager,
        EnemyTargetingActor->GetCharacterCoord(),
        PlayerTargetingActor->GetCharacterCoord());
    if (!IsValid(SelectedCard))
    {
        UE_LOG(LogTemp, Error, TEXT("[BattleTargetingManager] Enemy card selection failed."));
        return false;
    }

    if (TargetingPresentationController)
    {
        TargetingPresentationController->ClearExecutionPreview();
        TargetingPresentationController->ClearSession(EnemyTargetingSession);
    }
    else if (EnemyTargetingSession)
    {
        EnemyTargetingSession->EndSession();
        EnemyTargetingSession = nullptr;
    }

    EnemyTargetingSession = NewObject<UBattleTargetingSession>(this);
    if (!EnemyTargetingSession || !EnemyTargetingSession->StartSession(
        EnemyTargetingActor,
        RuntimeGridManager,
        ResolveRuntimeGridWorldType(),
        SelectedCard->TargetingData,
        false))
    {
        EnemyTargetingSession = nullptr;
        return false;
    }

    while (EnemyTargetingSession->IsSelecting())
    {
        const int32 StepIndex = EnemyTargetingSession->GetCurrentStepIndex();
        const FHexOffsetCoord PlayerCoord = PlayerTargetingActor->GetCharacterCoord();
        const FVector PlayerWorldLocation = RuntimeGridManager->GetWorldLocationByCoord(PlayerCoord);
        EnemyTargetingSession->UpdateAimWorldLocation(PlayerWorldLocation, true);

        if (!EnemyTargetingSession->UpdateCandidateCoord(PlayerCoord)
            || EnemyTargetingSession->ConfirmStep() == ETargetingConfirmResult::Failed)
        {
            UE_LOG(LogTemp, Error,
                TEXT("[BattleTargetingManager] Enemy card cannot target player. Step=%d Card=%s"),
                StepIndex,
                *GetNameSafe(SelectedCard));
            EnemyTargetingSession->EndSession();
            EnemyTargetingSession = nullptr;
            return false;
        }
    }

    if (!EnemyTargetingSession->IsCompleted())
    {
        return false;
    }

    FBattleAction BattleAction;
    BattleAction.ExchangeIndex = BattleManager->GetCurrentExchange();
    BattleAction.Card = SelectedCard;
    BattleAction.Attacker = EnemyCharacter;
    BattleAction.Speed = EnemyCharacter->GetCharacterSpeed() + SelectedCard->CardSpeed;
    BattleAction.bPlayerAction = false;
    BattleAction.TargetingIntent = EnemyTargetingSession->GetIntent();

    BattleRuntimeContext->SetEnemyExchangeAction(BattleAction.ExchangeIndex, BattleAction);
    return true;
}

void ABattleTargetingManager::NotifyEnemyCardSelectionUIFinished()
{
    if (!BattleRuntimeContext || !BattleManager
        || !BattleRuntimeContext->GetEnemyExchangeAction(BattleManager->GetCurrentExchange()))
    {
        return;
    }

    bEnemyCardSelectionFinished = true;
    TryFinishCurrentExchangeTargeting();
}

void ABattleTargetingManager::TryFinishCurrentExchangeTargeting()
{
    if (!bPlayerCardSelectionFinished || !bEnemyCardSelectionFinished || !BattleManager || !BattleRuntimeContext)
    {
        return;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    if (BattleManager->GetCurrentPhase() != EBattlePhase::Targeting
        || !BattleRuntimeContext->HasExchangeActions(ExchangeIndex))
    {
        return;
    }

    UBattlePhaseTask* CompletedTask = PhaseExecutionTask;
    PhaseExecutionTask = nullptr;
    if (CompletedTask) CompletedTask->Complete();
}

void ABattleTargetingManager::RefreshExchangeTargetIndicators()
{
    if (!BattleGridManager || !BattleRuntimeContext || !BattleManager)
    {
        return;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
    const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
    if (!PlayerAction || !EnemyAction)
    {
        return;
    }

    BattleGridManager->AllClearGridHovered();
    BattleGridManager->AllClearExchangeIndicator();

    FResolvedTargeting PlayerResolvedTargeting;
    if (ResolveActionTargetingForCurrentGrid(*PlayerAction, PlayerResolvedTargeting))
    {
        TArray<FHexOffsetCoord> TargetCoords = PlayerResolvedTargeting.AffectedCoords;
        if (TargetCoords.IsEmpty() && PlayerResolvedTargeting.HasSelectedCoord())
        {
            TargetCoords.Add(PlayerResolvedTargeting.GetSelectedCoord());
        }
        BattleGridManager->SetExchangeIndicator(PlayerAction->Card->CardTypeInfo, TargetCoords, false);
    }

    FResolvedTargeting EnemyResolvedTargeting;
    if (ResolveActionTargetingForCurrentGrid(*EnemyAction, EnemyResolvedTargeting))
    {
        TArray<FHexOffsetCoord> TargetCoords = EnemyResolvedTargeting.AffectedCoords;
        if (TargetCoords.IsEmpty() && EnemyResolvedTargeting.HasSelectedCoord())
        {
            TargetCoords.Add(EnemyResolvedTargeting.GetSelectedCoord());
        }
        BattleGridManager->SetExchangeIndicator(EnemyAction->Card->CardTypeInfo, TargetCoords, true);
    }
}

void ABattleTargetingManager::NotifyEnemyCardRevealUIFinished(int32 ExchangeIndex)
{
    if (!BattleManager || !BattleRuntimeContext
        || BattleManager->GetCurrentPhase() != EBattlePhase::CardReveal
        || ExchangeIndex != BattleManager->GetCurrentExchange())
    {
        return;
    }

    RefreshExchangeTargetIndicators();

    const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
    if (TargetingPresentationController && EnemyAction && IsValid(EnemyAction->Card))
    {
        TargetingPresentationController->ClearExecutionPreview();

        const int32 StepCount = EnemyAction->Card->TargetingData.Steps.Num();
        for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
        {
            const FTargetingStepCardData* StepData = EnemyAction->Card->TargetingData.GetStep(StepIndex);
            if (!StepData)
            {
                continue;
            }

            const FTargetingPhasePresentationSettings& PresentationSettings =
                StepData->AdvancedSettings.Presentation.AttackSequencePhase;
            const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview
                || PresentationSettings.bShowPathPreview
                || PresentationSettings.bShowAreaPreview;
            if (!bShowAnyPreview)
            {
                continue;
            }

            FResolvedTargeting StepResolvedTargeting;
            if (ResolveActionTargetingThroughStepForCurrentGrid(*EnemyAction, StepIndex, StepResolvedTargeting))
            {
                TargetingPresentationController->AddResolvedStepPreview(
                    ResolveRuntimeCharacter(EnemyAction->Attacker),
                    ResolveRuntimeGridWorldType(),
                    EnemyAction->Card->TargetingData,
                    StepResolvedTargeting,
                    StepIndex,
                    PresentationSettings,
                    true);
            }
        }
    }

    GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);
    GetWorldTimerManager().SetTimer(
        EnemyPreviewHideTimerHandle,
        this,
        &ABattleTargetingManager::HideEnemyTargetingPreviewAndFinishReveal,
        FMath::Max(EnemyPreviewDuration, 0.05f),
        false);
}

void ABattleTargetingManager::HideEnemyTargetingPreview()
{
    if (TargetingPresentationController)
    {
        TargetingPresentationController->ClearExecutionPreview();
    }

    if (EnemyTargetingSession)
    {
        EnemyTargetingSession->HidePreview();
    }
}

void ABattleTargetingManager::HideEnemyTargetingPreviewAndFinishReveal()
{
    HideEnemyTargetingPreview();
    ClearSelectionAndRevealPreviews();
    PendingPlayerCard = nullptr;

    if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::CardReveal)
    {
        UBattlePhaseTask* CompletedTask = PhaseUITask;
        PhaseUITask = nullptr;
        if (CompletedTask) CompletedTask->Complete();
    }
}

bool ABattleTargetingManager::ResolveGridCoordFromHit(
    const FHitResult& HitResult,
    FHexOffsetCoord& OutCoord) const
{
    OutCoord = FHexOffsetCoord::Invalid();

    ABattleGridManager* RuntimeGridManager = ResolveRuntimeGridManager();

    if (!IsValid(RuntimeGridManager))
    {
        return false;
    }

    AActor* HitActor = HitResult.GetActor();
    if (const ABattleGridTile* HitTile = Cast<ABattleGridTile>(HitActor))
    {
        OutCoord = HitTile->GetGridCoord();
        return true;
    }

    if (ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitActor))
    {
        ABattleCharacterBase* RuntimeCharacter = ResolveRuntimeCharacter(HitCharacter);
        OutCoord = RuntimeCharacter ? RuntimeCharacter->GetCharacterCoord() : FHexOffsetCoord::Invalid();
        return OutCoord.IsValid();
    }

    float BestDistanceSquared = TNumericLimits<float>::Max();
    for (int32 X = 0; X < RuntimeGridManager->GetGridWidth(); ++X)
    {
        for (int32 Y = 0; Y < RuntimeGridManager->GetGridHeight(); ++Y)
        {
            const FHexOffsetCoord Coord(X, Y);
            FVector PresentationLocation = FVector::ZeroVector;
            if (!RuntimeGridManager->GetPresentationWorldLocationByCoord(Coord, PresentationLocation)) continue;
            const FVector Delta = PresentationLocation - HitResult.ImpactPoint;
            const float DistanceSquared = FVector2D(Delta.X, Delta.Y).SizeSquared();
            if (DistanceSquared < BestDistanceSquared)
            {
                BestDistanceSquared = DistanceSquared;
                OutCoord = Coord;
            }
        }
    }

    return OutCoord.IsValid();
}

ABattleGridManager* ABattleTargetingManager::ResolveRuntimeGridManager() const
{
    // 모든 WorldType은 하나의 GridManager가 소유한다.
    // 런타임/시뮬레이션의 차이는 ResolveRuntimeGridWorldType()으로만 구분한다.
    return BattleGridManager.Get();
}

EBattleSimulationWorldType ABattleTargetingManager::ResolveRuntimeGridWorldType() const
{
    return IsValid(BattleSimulationManager) && BattleSimulationManager->IsSimulationRunning()
        ? EBattleSimulationWorldType::PlayerActualEnemyDeceived
        : EBattleSimulationWorldType::PlayerActualEnemyActual;
}

ABattleCharacterBase* ABattleTargetingManager::ResolveRuntimeCharacter(const ABattleCharacterBase* SourceCharacter) const
{
    if (!IsValid(SourceCharacter)) return nullptr;
    if (!IsValid(BattleSimulationManager) || !BattleSimulationManager->IsSimulationRunning()) return const_cast<ABattleCharacterBase*>(SourceCharacter);
    UBattleSimulationWorldRuntime* WorldRuntime = BattleSimulationManager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerActualEnemyDeceived);
    ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldRuntime) ? WorldRuntime->GetSimulationCharacter(SourceCharacter) : nullptr;
    return IsValid(SimulationCharacter) ? SimulationCharacter : const_cast<ABattleCharacterBase*>(SourceCharacter);
}

ABattleCharacterBase* ABattleTargetingManager::GetPlayerTargetingActor() const
{
    return BattleRuntimeContext ? ResolveRuntimeCharacter(BattleRuntimeContext->GetPlayerCharacter()) : nullptr;
}

ABattleCharacterBase* ABattleTargetingManager::GetEnemyTargetingActor() const
{
    return BattleRuntimeContext ? ResolveRuntimeCharacter(BattleRuntimeContext->GetEnemyCharacter()) : nullptr;
}

bool ABattleTargetingManager::ResolveActionTargetingForCurrentGrid(
    const FBattleAction& Action,
    FResolvedTargeting& OutResolvedTargeting) const
{
    FBattleAction RuntimeAction = Action;
    RuntimeAction.Attacker = ResolveRuntimeCharacter(Action.Attacker);

    return FBattleTargetResolver::ResolveAction(RuntimeAction, ResolveRuntimeGridManager(), ResolveRuntimeGridWorldType(), OutResolvedTargeting);
}

bool ABattleTargetingManager::ResolveActionTargetingThroughStepForCurrentGrid(
    const FBattleAction& Action,
    int32 LastStepIndex,
    FResolvedTargeting& OutResolvedTargeting) const
{
    FBattleAction RuntimeAction = Action;
    RuntimeAction.Attacker = ResolveRuntimeCharacter(Action.Attacker);

    return FBattleTargetResolver::ResolveActionThroughStep(
        RuntimeAction,
        ResolveRuntimeGridManager(),
        ResolveRuntimeGridWorldType(),
        LastStepIndex,
        OutResolvedTargeting);
}
