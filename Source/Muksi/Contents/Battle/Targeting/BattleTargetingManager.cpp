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
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

ABattleTargetingManager::ABattleTargetingManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ABattleTargetingManager::BeginPlay()
{
    Super::BeginPlay();

    TargetingPresentationController = NewObject<UTargetingPresentationController>(this);

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
    {
        ManagerSubsystem->RegisterManager<ABattleTargetingManager>(this);
    }

    if (!TryBindBattleFlow())
    {
        BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
            FTimerDelegate::CreateUObject(this, &ABattleTargetingManager::BindBattleFlowDeferred));
    }
}

void ABattleTargetingManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(BattleFlowBindingTimerHandle);
    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);
    GetWorldTimerManager().ClearTimer(EnemyPreviewHideTimerHandle);

    if (BattleManager)
    {
        BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandleBattlePhaseChanged);
    }

    ClearSelectionAndRevealPreviews();
    PendingPlayerCard = nullptr;
    BattleRuntimeContext = nullptr;
    BattleGridManager = nullptr;
    BattleManager = nullptr;

    if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
    {
        ManagerSubsystem->UnregisterManager<ABattleTargetingManager>(this);
    }

    Super::EndPlay(EndPlayReason);
}

bool ABattleTargetingManager::TryBindBattleFlow()
{
    UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
    if (!ManagerSubsystem)
    {
        return false;
    }

    ABattleManager* FoundBattleManager = ManagerSubsystem->GetManager<ABattleManager>();
    if (!IsValid(FoundBattleManager) || !IsValid(FoundBattleManager->GetBattleRuntimeContext()))
    {
        return false;
    }

    if (BattleManager && BattleManager != FoundBattleManager)
    {
        BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandleBattlePhaseChanged);
    }

    BattleManager = FoundBattleManager;
    BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    BattleGridManager = ManagerSubsystem->GetManager<ABattleGridManager>();

    if (!IsValid(BattleGridManager))
    {
        return false;
    }

    if (TargetingPresentationController)
    {
        TargetingPresentationController->Initialize(BattleGridManager);
    }

    BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleTargetingManager::HandleBattlePhaseChanged);
    BattleManager->ChangePhaseDelegate.AddUniqueDynamic(this, &ABattleTargetingManager::HandleBattlePhaseChanged);
    return true;
}

void ABattleTargetingManager::BindBattleFlowDeferred()
{
    if (TryBindBattleFlow())
    {
        return;
    }

    BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
        FTimerDelegate::CreateUObject(this, &ABattleTargetingManager::BindBattleFlowDeferred));
}

void ABattleTargetingManager::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
    (void)OldPhase;

    switch (NewPhase)
    {
    case EBattlePhase::RoundStart:
    case EBattlePhase::CardSelect:
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
    if ((!IsValid(BattleManager) || !IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager)) && !TryBindBattleFlow())
    {
        return false;
    }

    if (!IsValid(CardData) || BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect)
    {
        return false;
    }

    PendingPlayerCard = CardData;
    BattleManager->NotifyInteractivePhaseFinished(EBattlePhase::CardSelect);

    // Phase delegate broadcast가 끝난 뒤 즉시 완료 카드를 처리해 재진입 전환을 피한다.
    if (PlayerTargetingSession && PlayerTargetingSession->IsCompleted())
    {
        NotifyPlayerCardSelectionFinished();
    }

    return IsPlayerCardTargeting() || bPlayerCardSelectionFinished;
}

bool ABattleTargetingManager::StartPendingPlayerTargeting()
{
    if (!IsValid(PendingPlayerCard) || !IsValid(BattleGridManager))
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
        BattleGridManager,
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

bool ABattleTargetingManager::RequestUpdatePlayerTargeting(
    const FHitResult& HitResult,
    bool bHasHitResult)
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
    if ((!IsValid(BattleManager) || !IsValid(BattleRuntimeContext)) && !TryBindBattleFlow())
    {
        return false;
    }

    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
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

    OnEnemyCardSelectionReady.Broadcast(EnemyAction->Card.Get(), ExchangeIndex);
}

bool ABattleTargetingManager::BuildEnemyActionForCurrentExchange()
{
    if ((!IsValid(BattleManager) || !IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager)) && !TryBindBattleFlow())
    {
        return false;
    }

    if (BattleManager->GetCurrentPhase() != EBattlePhase::CardSelect
        && BattleManager->GetCurrentPhase() != EBattlePhase::Targeting)
    {
        return false;
    }

    ABattleCharacter_Enemy* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();
    ABattleCharacterBase* EnemyTargetingActor = GetEnemyTargetingActor();
    ABattleCharacterBase* PlayerTargetingActor = GetPlayerTargetingActor();
    if (!IsValid(EnemyCharacter) || !EnemyTargetingActor || !PlayerTargetingActor)
    {
        return false;
    }

    UMuksiBattleCardDataAsset* SelectedCard = EnemyCharacter->SelectCardForExchange(
        BattleGridManager,
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
        BattleGridManager,
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
        const FVector PlayerWorldLocation = BattleGridManager->GetWorldLocationByCoord(PlayerCoord);
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

    BattleManager->NotifyInteractivePhaseFinished(EBattlePhase::Targeting);
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
        BattleGridManager->SetExchangeIndicator(PlayerAction->Card->AttackType.AttackType, TargetCoords, false);
    }

    FResolvedTargeting EnemyResolvedTargeting;
    if (ResolveActionTargetingForCurrentGrid(*EnemyAction, EnemyResolvedTargeting))
    {
        TArray<FHexOffsetCoord> TargetCoords = EnemyResolvedTargeting.AffectedCoords;
        if (TargetCoords.IsEmpty() && EnemyResolvedTargeting.HasSelectedCoord())
        {
            TargetCoords.Add(EnemyResolvedTargeting.GetSelectedCoord());
        }
        BattleGridManager->SetExchangeIndicator(EnemyAction->Card->AttackType.AttackType, TargetCoords, true);
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
                    BattleRuntimeContext->ResolveRuntimeCharacter(EnemyAction->Attacker),
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
        BattleManager->NotifyInteractivePhaseFinished(EBattlePhase::CardReveal);
    }
}

bool ABattleTargetingManager::ResolveGridCoordFromHit(
    const FHitResult& HitResult,
    FHexOffsetCoord& OutCoord) const
{
    OutCoord = FHexOffsetCoord::Invalid();

    if (!BattleGridManager)
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
        ABattleCharacterBase* RuntimeCharacter = BattleRuntimeContext
            ? BattleRuntimeContext->ResolveRuntimeCharacter(HitCharacter)
            : HitCharacter;
        OutCoord = RuntimeCharacter ? RuntimeCharacter->GetCharacterCoord() : FHexOffsetCoord::Invalid();
        return OutCoord.IsValid();
    }

    float BestDistanceSquared = TNumericLimits<float>::Max();
    for (int32 X = 0; X < BattleGridManager->GetGridWidth(); ++X)
    {
        for (int32 Y = 0; Y < BattleGridManager->GetGridHeight(); ++Y)
        {
            const FHexOffsetCoord Coord(X, Y);
            if (!BattleGridManager->GetTileActorByCoord(Coord))
            {
                continue;
            }

            const FVector Delta = BattleGridManager->GetWorldLocationByCoord(Coord) - HitResult.ImpactPoint;
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

ABattleCharacterBase* ABattleTargetingManager::GetPlayerTargetingActor() const
{
    if (!BattleRuntimeContext)
    {
        return nullptr;
    }

    return BattleRuntimeContext->ResolveRuntimeCharacter(
        BattleRuntimeContext->GetPlayerCharacter());
}

ABattleCharacterBase* ABattleTargetingManager::GetEnemyTargetingActor() const
{
    if (!BattleRuntimeContext)
    {
        return nullptr;
    }

    return BattleRuntimeContext->ResolveRuntimeCharacter(
        BattleRuntimeContext->GetEnemyCharacter());
}

bool ABattleTargetingManager::ResolveActionTargetingForCurrentGrid(
    const FBattleAction& Action,
    FResolvedTargeting& OutResolvedTargeting) const
{
    FBattleAction RuntimeAction = Action;
    if (BattleRuntimeContext)
    {
        RuntimeAction.Attacker = BattleRuntimeContext->ResolveRuntimeCharacter(Action.Attacker);
    }

    return FBattleTargetResolver::ResolveAction(RuntimeAction, BattleGridManager, OutResolvedTargeting);
}

bool ABattleTargetingManager::ResolveActionTargetingThroughStepForCurrentGrid(
    const FBattleAction& Action,
    int32 LastStepIndex,
    FResolvedTargeting& OutResolvedTargeting) const
{
    FBattleAction RuntimeAction = Action;
    if (BattleRuntimeContext)
    {
        RuntimeAction.Attacker = BattleRuntimeContext->ResolveRuntimeCharacter(Action.Attacker);
    }

    return FBattleTargetResolver::ResolveActionThroughStep(
        RuntimeAction,
        BattleGridManager,
        LastStepIndex,
        OutResolvedTargeting);
}
