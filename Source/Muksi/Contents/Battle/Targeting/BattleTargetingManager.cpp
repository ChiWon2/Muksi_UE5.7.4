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
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedStepResult.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"

namespace
{
    constexpr EBattleSimulationWorldType PlayerTargetingWorldType = EBattleSimulationWorldType::PlayerActualEnemyDeceived;
    constexpr EBattleSimulationWorldType EnemyTargetingWorldType = EBattleSimulationWorldType::PlayerDeceivedEnemyActual;
}

ABattleTargetingManager::ABattleTargetingManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleTargetingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().ClearTimer(CardRevealPreviewTimerHandle);

    if (TargetingPresentationController)
        ClearAllTargeting();

    if (BattleManager)
        BattleManager->PhaseEntryRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseEntryRequested);

    if (BattleManager)
        BattleManager->PhaseUIRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseUIRequested);

    if (BattleManager)
        BattleManager->PhaseExecutionRequestedDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandlePhaseExecutionRequested);

    CardRevealTask = nullptr;
    CardSelectTask = nullptr;
    TargetingTask = nullptr;
    PlayerTargetingCard = nullptr;
    BattleManager = nullptr;

    Super::EndPlay(EndPlayReason);
}

bool ABattleTargetingManager::InitializeBattleFlow(ABattleManager* InBattleManager)
{
    if (!IsValid(InBattleManager))
    {
        return false;
    }

    if (!IsValid(InBattleManager->GetBattleRuntimeContext()) || !IsValid(InBattleManager->GetBattleGridManager()) || !IsValid(InBattleManager->GetBattleSimulationManager()))
    {
        return false;
    }

    BattleManager = InBattleManager;
    BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseEntryRequested);
    BattleManager->PhaseUIRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseUIRequested);
    BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandlePhaseExecutionRequested);

    if (!TargetingPresentationController)
        TargetingPresentationController = NewObject<UTargetingPresentationController>(this);

    if (!TargetingPresentationController)
        return false;

    TargetingPresentationController->Initialize(BattleManager->GetBattleGridManager());

    return true;
}

void ABattleTargetingManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)TaskContext;

    switch (NewPhase)
    {
    case EBattlePhase::CardSelect:
        ResetPlayerTargeting();

        if (OldPhase != EBattlePhase::Targeting)
        {
            ResetEnemyTargeting();
        }
        break;

    case EBattlePhase::Targeting:
        if (!StartPlayerTargeting())
        {
            UE_LOG(LogTemp, Error, TEXT("[BattleTargetingManager] Failed to start player targeting."));
            GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                if (BattleManager->GetCurrentPhase() == EBattlePhase::Targeting)
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
        ClearAllTargeting();
        PlayerTargetingCard = nullptr;
        break;

    default:
        break;
    }

}

void ABattleTargetingManager::HandlePhaseUIRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)OldPhase;
    if (NewPhase != EBattlePhase::CardReveal || !TaskContext) 
        return;
    CardRevealTask = TaskContext->RegisterTask(this);
}

void ABattleTargetingManager::HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
    (void)OldPhase;
    if (!TaskContext)
    {
        return;
    }

    if (NewPhase == EBattlePhase::CardSelect)
    {
        CardSelectTask = TaskContext->RegisterTask(this);
        return;
    }

    if (NewPhase == EBattlePhase::Targeting)
    {
        TargetingTask = TaskContext->RegisterTask(this);
    }
}

void ABattleTargetingManager::ResetPlayerTargeting()
{
    TargetingTask = nullptr;
    PlayerTargetingSession = nullptr;
    TargetingPresentationController->ClearLivePreview();

    PlayerTargetingCard = nullptr;

    BattleManager->GetBattleRuntimeContext()->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());
}

void ABattleTargetingManager::ResetEnemyTargeting()
{
    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().ClearTimer(CardRevealPreviewTimerHandle);
    EnemyTargetingSession = nullptr;
    TargetingPresentationController->ClearAllPresentation();

    bEnemyCardPresentationFinished = false;

    BattleManager->GetBattleRuntimeContext()->ClearEnemyExchangeAction(BattleManager->GetCurrentExchange());
}

void ABattleTargetingManager::BuildAndPresentPlayerTargetingPreview()
{
    // Targeting 계산은 Manager/PatternProcessor에서 끝내고, PresentationController에는 표시할 Context만 전달한다.
    if (!PlayerTargetingSession || !PlayerTargetingCard)
        return;

    const FTargetingStepCardData* StepData = PlayerTargetingSession->GetCurrentStepData();
    if (!StepData)
    {
        TargetingPresentationController->ClearLivePreview();
        return;
    }

    FSelectionStepResult PreviewStepResult = PlayerTargetingSession->GetCurrentStepResult();

    if (!PreviewStepResult.HasOriginCoord())
        PlayerTargetingSession->GetCurrentOriginCoord(PreviewStepResult.OriginCoord);

    TArray<FHexOffsetCoord> PreviewAffectedCoords;
    TArray<FHexOffsetCoord> PreviewPathCoords;
    FTargetingPreviewContext PreviewContext;
    PreviewContext.GridManager = BattleManager->GetBattleGridManager();
    PreviewContext.StepData = StepData;
    PreviewContext.SelectionStep = &PreviewStepResult;

    if (PreviewStepResult.bValid && PreviewStepResult.HasSelectedCoord())
    {
        if (!StepData->Pattern.PatternClass)
        {
            PreviewAffectedCoords.Add(PreviewStepResult.SelectedCoord);
        }
        else
        {
            const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
            if (Pattern)
            {
                Pattern->ApplyPattern(
                    PreviewContext.GridManager,
                    PlayerTargetingWorldType,
                    StepData->Pattern.PatternData,
                    PreviewStepResult.OriginCoord,
                    PreviewStepResult.SelectedCoord,
                    PreviewStepResult.SelectedDirection,
                    PreviewAffectedCoords,
                    PreviewPathCoords);
            }
        }

        PreviewContext.PreviewAffectedCoords = &PreviewAffectedCoords;
        PreviewContext.PreviewPathCoords = &PreviewPathCoords;
    }

    if (!PreviewContext.IsValid())
    {
        TargetingPresentationController->ClearLivePreview();
        return;
    }

    TargetingPresentationController->UpdateLivePreview(*StepData, PreviewContext);
}

void ABattleTargetingManager::ClearAllTargeting()
{
    PlayerTargetingSession = nullptr;
    EnemyTargetingSession = nullptr;

    TargetingPresentationController->ClearAllPresentation();
}

bool ABattleTargetingManager::RequestPlayerCardSelection(UMuksiBattleCardDataAsset* CardData)
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (!IsValid(CardData) || BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect)
    {
        return false;
    }

    PlayerTargetingCard = CardData;
    UBattlePhaseTask* CompletedTask = CardSelectTask;
    CardSelectTask = nullptr;
    if (CompletedTask)
    {
        CompletedTask->Complete();
    }

    // Phase delegate broadcast가 끝난 뒤 즉시 완료 카드를 처리해 재진입 전환을 피한다.
    if (PlayerTargetingSession && PlayerTargetingSession->IsCompleted())
    {
        CompletePlayerTargeting();
    }

    const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(BattleManager->GetCurrentExchange());
    return CanProcessPlayerTargetingInput() || PlayerAction != nullptr;
}

bool ABattleTargetingManager::StartPlayerTargeting()
{
    if (!IsValid(PlayerTargetingCard))
        return false;

    ABattleGridManager* RuntimeGridManager = BattleManager->GetBattleGridManager();

    TargetingPresentationController->ClearTargetingResultPreviews();
    PlayerTargetingSession = nullptr;

    ABattleCharacterBase* PlayerTargetingActor = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(BattleManager->GetBattleRuntimeContext()->GetPlayerCharacter(), PlayerTargetingWorldType);
    if (!PlayerTargetingActor)
    {
        return false;
    }

    PlayerTargetingSession = NewObject<UBattleTargetingSession>(this);
    if (!PlayerTargetingSession || !PlayerTargetingSession->StartSession(PlayerTargetingActor, RuntimeGridManager, PlayerTargetingWorldType, PlayerTargetingCard->TargetingData))
    {
        PlayerTargetingSession = nullptr;
        return false;
    }

    if (!TargetingPresentationController->StartLivePreview(PlayerTargetingActor, PlayerTargetingWorldType))
    {
        PlayerTargetingSession = nullptr;
        return false;
    }

    BuildAndPresentPlayerTargetingPreview();
    return true;
}

bool ABattleTargetingManager::RequestConfirmPlayerTargeting()
{
    if (!CanProcessPlayerTargetingInput())
    {
        return false;
    }

    const ETargetingConfirmResult ConfirmResult = PlayerTargetingSession->ConfirmStep();
    BuildAndPresentPlayerTargetingPreview();
    if (ConfirmResult == ETargetingConfirmResult::Completed)
    {
        CompletePlayerTargeting();
    }

    return ConfirmResult != ETargetingConfirmResult::Failed;
}

bool ABattleTargetingManager::RequestUndoOrCancelPlayerTargeting()
{
    if (!CanProcessPlayerTargetingInput())
    {
        return false;
    }

    if (!UndoPlayerCardTargetingStep())
    {
        CancelPlayerTargeting();
    }

    return true;
}

bool ABattleTargetingManager::RequestUpdatePlayerTargeting(const FHitResult& HitResult, bool bHasHitResult)
{
    if (!CanProcessPlayerTargetingInput())
    {
        return false;
    }

    if (!bHasHitResult)
    {
        PlayerTargetingSession->UpdateSelection(FHexOffsetCoord::Invalid(), INDEX_NONE);
        BuildAndPresentPlayerTargetingPreview();
        return true;
    }

    FHexOffsetCoord CandidateCoord = FHexOffsetCoord::Invalid();
    GetGridCoordFromHit(HitResult, CandidateCoord);
    const int32 Direction = CalculateDirectionToCoord(PlayerTargetingSession, CandidateCoord);
    PlayerTargetingSession->UpdateSelection(CandidateCoord, Direction);
    BuildAndPresentPlayerTargetingPreview();
    return true;
}

bool ABattleTargetingManager::UndoPlayerCardTargetingStep()
{
    if (!PlayerTargetingSession || BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    if (!PlayerTargetingSession->UndoStep())
    {
        return false;
    }

    BuildAndPresentPlayerTargetingPreview();

    BattleManager->GetBattleRuntimeContext()->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());

    UE_LOG(LogTemp, Log, TEXT("[BattleTargetingManager] Player targeting undo. RestoredStep=%d"),
        PlayerTargetingSession->GetCurrentStepIndex());
    return true;
}

void ABattleTargetingManager::CancelPlayerTargeting()
{
    PlayerTargetingSession = nullptr;
    TargetingPresentationController->ClearLivePreview();

    PlayerTargetingCard = nullptr;

    BattleManager->GetBattleRuntimeContext()->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());

    if (BattleManager->GetCurrentPhase() == EBattlePhase::Targeting)
    {
        BattleManager->RestartCurrentExchangeCardSelection();
    }
}

bool ABattleTargetingManager::CanProcessPlayerTargetingInput() const
{
    return PlayerTargetingSession
        && BattleManager->GetCurrentPhase() == EBattlePhase::Targeting
        && PlayerTargetingSession->IsSelecting();
}

bool ABattleTargetingManager::CompletePlayerTargeting()
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (!PlayerTargetingSession || !PlayerTargetingSession->IsCompleted() || !IsValid(PlayerTargetingCard))
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
    BattleAction.Card = PlayerTargetingCard;
    BattleAction.Speed = PlayerCharacter->GetCharacterSpeed() + PlayerTargetingCard->CardSpeed;
    BattleAction.Attacker = PlayerCharacter;
    BattleAction.bPlayerAction = true;
    BattleAction.TargetingIntent = PlayerTargetingSession->GetIntent();

    BattleRuntimeContext->SetPlayerExchangeAction(BattleAction.ExchangeIndex, BattleAction);
    TryCompleteTargetingPhase();
    return true;
}

bool ABattleTargetingManager::RequestEnemyCardSelection()
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    if (BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex))
    {
        return true;
    }

    if (GetWorldTimerManager().IsTimerActive(EnemyCardSelectionTimerHandle))
    {
        return true;
    }

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
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    FBattleAction EnemyAction;
    if (!BuildEnemyAction(EnemyAction))
    {
        return;
    }

    BattleRuntimeContext->SetEnemyExchangeAction(EnemyAction.ExchangeIndex, EnemyAction);

    UMuksiBattleCardDataAsset* PresentedCard = EnemyAction.Card.Get();
    UMuksiBattleCardDataAsset* DeceivedCard = EnemyAction.Card->GetDeceivedCard();
    if (IsValid(DeceivedCard))
    {
        PresentedCard = DeceivedCard;
    }

    OnEnemyCardSelectionReady.Broadcast(PresentedCard, EnemyAction.ExchangeIndex);
}

bool ABattleTargetingManager::BuildEnemyAction(FBattleAction& OutAction)
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    ABattleGridManager* RuntimeGridManager = BattleManager->GetBattleGridManager();
    ABattleCharacter_Enemy* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();
    ABattleCharacterBase* EnemyTargetingActor = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(EnemyCharacter, EnemyTargetingWorldType);
    ABattleCharacterBase* PlayerTargetingActor = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(BattleRuntimeContext->GetPlayerCharacter(), EnemyTargetingWorldType);
    if (!IsValid(EnemyCharacter) || !EnemyTargetingActor || !PlayerTargetingActor)
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

    TargetingPresentationController->ClearTargetingResultPreviews();
    EnemyTargetingSession = NewObject<UBattleTargetingSession>(this);
    if (!EnemyTargetingSession || !EnemyTargetingSession->StartSession(EnemyTargetingActor,RuntimeGridManager, EnemyTargetingWorldType,SelectedCard->TargetingData))
    {
        EnemyTargetingSession = nullptr;
        return false;
    }

    if (!CompleteEnemyTargetingSession(SelectedCard, PlayerTargetingActor))
    {
        EnemyTargetingSession = nullptr;
        return false;
    }

    OutAction.ExchangeIndex = BattleManager->GetCurrentExchange();
    OutAction.Card = SelectedCard;
    OutAction.Attacker = EnemyCharacter;
    OutAction.Speed = EnemyCharacter->GetCharacterSpeed() + SelectedCard->CardSpeed;
    OutAction.bPlayerAction = false;
    OutAction.TargetingIntent = EnemyTargetingSession->GetIntent();
    return true;
}

bool ABattleTargetingManager::CompleteEnemyTargetingSession(UMuksiBattleCardDataAsset* SelectedCard, ABattleCharacterBase* TargetCharacter)
{
    if (!EnemyTargetingSession || !IsValid(SelectedCard) || !IsValid(TargetCharacter))
    {
        return false;
    }

    while (EnemyTargetingSession->IsSelecting())
    {
        const int32 StepIndex = EnemyTargetingSession->GetCurrentStepIndex();
        const FHexOffsetCoord TargetCoord = TargetCharacter->GetCharacterCoord();
        const int32 Direction = CalculateDirectionToCoord(EnemyTargetingSession, TargetCoord);
        if (!EnemyTargetingSession->UpdateSelection(TargetCoord, Direction)
            || EnemyTargetingSession->ConfirmStep() == ETargetingConfirmResult::Failed)
        {
            UE_LOG(LogTemp, Error,
                TEXT("[BattleTargetingManager] Enemy card cannot target character. Step=%d Card=%s"),
                StepIndex,
                *GetNameSafe(SelectedCard));
            return false;
        }
    }

    return EnemyTargetingSession->IsCompleted();
}

void ABattleTargetingManager::NotifyEnemyCardSelectionUIFinished()
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (!BattleRuntimeContext->GetEnemyExchangeAction(BattleManager->GetCurrentExchange()))
    {
        return;
    }

    bEnemyCardPresentationFinished = true;
    TryCompleteTargetingPhase();
}

void ABattleTargetingManager::TryCompleteTargetingPhase()
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    if (!bEnemyCardPresentationFinished)
    {
        return;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();
    if (BattleManager->GetCurrentPhase() != EBattlePhase::Targeting
        || !BattleRuntimeContext->HasExchangeActions(ExchangeIndex))
    {
        return;
    }

    UBattlePhaseTask* CompletedTask = TargetingTask;
    TargetingTask = nullptr;
    if (CompletedTask)
    {
        CompletedTask->Complete();
    }
}

void ABattleTargetingManager::NotifyEnemyCardRevealUIFinished(int32 ExchangeIndex)
{
    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardReveal
        || ExchangeIndex != BattleManager->GetCurrentExchange())
    {
        return;
    }

    StartCardRevealPresentation(ExchangeIndex);
}

void ABattleTargetingManager::StartCardRevealPresentation(int32 ExchangeIndex)
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
    const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);

    TargetingPresentationController->ClearLivePreview();
    ClearCardRevealPresentation();

    if (PlayerAction)
        ShowCardRevealActionPresentation(*PlayerAction);
    if (EnemyAction)
        ShowCardRevealActionPresentation(*EnemyAction);

    if (!PlayerAction && !EnemyAction)
    {
        FinishCardRevealPresentation();
        return;
    }

    GetWorldTimerManager().ClearTimer(CardRevealPreviewTimerHandle);
    GetWorldTimerManager().SetTimer(
        CardRevealPreviewTimerHandle,
        this,
        &ABattleTargetingManager::FinishCardRevealPresentation,
        FMath::Max(CardRevealPreviewDuration, 0.05f),
        false);
}

void ABattleTargetingManager::ShowCardRevealActionPresentation(const FBattleAction& Action)
{
    if (!IsValid(Action.Card))
        return;

    FTargetingResult TargetingResult;
    if (!BuildCardRevealTargetingResult(Action, TargetingResult))
        return;

    const int32 StepCount = Action.Card->TargetingData.Steps.Num();
    for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
    {
        const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
        if (!StepData || !TargetingResult.GetStep(StepIndex))
            continue;

        const FTargetingPhasePresentationSettings& PresentationSettings = StepData->Presentation.Phases.CardReveal;
        const bool bShowAnyPresentation = PresentationSettings.HasAnyPresentation();
        if (!bShowAnyPresentation)
            continue;

        TargetingPresentationController->AddTargetingResultPreview(
            BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(Action.Attacker, PlayerTargetingWorldType),
            PlayerTargetingWorldType,
            Action.Card->TargetingData,
            TargetingResult,
            StepIndex,
            PresentationSettings,
            !Action.bPlayerAction);
    }
}

void ABattleTargetingManager::ClearCardRevealPresentation()
{
    TargetingPresentationController->ClearTargetingResultPreviews();
}

void ABattleTargetingManager::FinishCardRevealPresentation()
{
    GetWorldTimerManager().ClearTimer(CardRevealPreviewTimerHandle);
    ClearCardRevealPresentation();

    ClearAllTargeting();
    PlayerTargetingCard = nullptr;

    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardReveal)
        return;

    UBattlePhaseTask* CompletedTask = CardRevealTask;
    CardRevealTask = nullptr;
    if (CompletedTask)
        CompletedTask->Complete();
}

int32 ABattleTargetingManager::CalculateDirectionToCoord(const UBattleTargetingSession* Session, const FHexOffsetCoord& TargetCoord) const
{
    if (!Session || !TargetCoord.IsValid())
        return INDEX_NONE;

    FHexOffsetCoord OriginCoord;

    if (!Session->GetCurrentOriginCoord(OriginCoord))
        return INDEX_NONE;

    return FHexGridMath::GetClosestDirection(OriginCoord, TargetCoord);
}

bool ABattleTargetingManager::GetGridCoordFromHit(
    const FHitResult& HitResult,
    FHexOffsetCoord& OutCoord) const
{
    OutCoord = FHexOffsetCoord::Invalid();

    ABattleGridManager* RuntimeGridManager = BattleManager->GetBattleGridManager();

    AActor* HitActor = HitResult.GetActor();
    if (const ABattleGridTile* HitTile = Cast<ABattleGridTile>(HitActor))
    {
        OutCoord = HitTile->GetGridCoord();
        return true;
    }

    if (ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitActor))
    {
        ABattleCharacterBase* RuntimeCharacter = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(HitCharacter, PlayerTargetingWorldType);
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
            if (!RuntimeGridManager->GetPresentationWorldLocationByCoord(Coord, PresentationLocation))
            {
                continue;
            }
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

bool ABattleTargetingManager::BuildCardRevealTargetingResult(
    const FBattleAction& Action,
    FTargetingResult& OutTargetingResult) const
{
    constexpr EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyDeceived;
    FBattleAction RuntimeAction = Action;
    RuntimeAction.Attacker = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(Action.Attacker, WorldType);
    if (!IsValid(RuntimeAction.Attacker))
        return false;

    OutTargetingResult.Reset();
    ABattleGridManager* GridManager = BattleManager->GetBattleGridManager();
    if (!IsValid(GridManager) || !IsValid(RuntimeAction.Card.Get()))
        return false;

    const FTargetingCardData& TargetingData = RuntimeAction.Card->TargetingData;
    TArray<FResolvedStepResult> ResolvedSteps;
    if (!FBattleTargetResolver::ResolveIntent(RuntimeAction.Attacker.Get(), GridManager, WorldType, TargetingData, RuntimeAction.TargetingIntent, ResolvedSteps))
        return false;

    OutTargetingResult.Steps.Reserve(ResolvedSteps.Num());
    for (int32 StepIndex = 0; StepIndex < ResolvedSteps.Num(); ++StepIndex)
    {
        const FTargetingStepCardData* StepData = TargetingData.GetStep(StepIndex);
        if (!StepData)
            return false;

        FTargetingStepResult StepResult;
        StepResult.ResolvedStep = ResolvedSteps[StepIndex];
        if (!StepResult.ResolvedStep.HasResolvedCoord())
            return false;

        if (!StepData->Pattern.PatternClass)
        {
            StepResult.AffectedCoords.Add(StepResult.ResolvedStep.ResolvedCoord);
        }
        else
        {
            const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
            if (!Pattern)
                return false;

            Pattern->ApplyPattern(
                GridManager,
                WorldType,
                StepData->Pattern.PatternData,
                StepResult.ResolvedStep.OriginCoord,
                StepResult.ResolvedStep.ResolvedCoord,
                StepResult.ResolvedStep.ResolvedDirection,
                StepResult.AffectedCoords,
                StepResult.PathCoords);
        }

        GridManager->GetCharactersAtCoords(WorldType, StepResult.AffectedCoords, StepResult.Targets);
        OutTargetingResult.Steps.Add(MoveTemp(StepResult));
    }
    return true;
}
