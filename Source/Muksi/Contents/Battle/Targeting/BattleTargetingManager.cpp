#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/Panic/PanicStrategyBase.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
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
            RequestEnemyCardSelection();
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
    TargetingPresentationController->ClearCurrentStepPreview();

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

void ABattleTargetingManager::RefreshTargetingPresentation(const UBattleTargetingSession* Session)
{
    if (!Session)
    {
        TargetingPresentationController->ClearCurrentStepPreview();
        return;
    }

    const FTargetingStepCardData* StepData = Session->GetCurrentStepData();
    if (!StepData)
    {
        TargetingPresentationController->ClearCurrentStepPreview();
        return;
    }

    const FTargetingStepResult& TargetingStep = Session->GetCurrentStepResult();
    if (!TargetingStep.Step.HasOriginCoord() || !TargetingStep.Step.HasTargetCoord())
    {
        TargetingPresentationController->ClearCurrentStepPreview();
        return;
    }

    FTargetingPreviewContext PreviewContext;
    PreviewContext.SourceCharacter = Session->GetSourceCharacter();
    PreviewContext.GridManager = BattleManager->GetBattleGridManager();
    PreviewContext.StepData = StepData;
    PreviewContext.TargetingStep = &TargetingStep;
    PreviewContext.PresentationSettings = &StepData->Presentation.Phases.Targeting;

    TargetingPresentationController->PresentCurrentStep(PreviewContext);
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

bool ABattleTargetingManager::RequestPlayerPanicTargeting(UMuksiBattleCardDataAsset* PanicCard)
{
    if (!BattleManager || !IsValid(PanicCard))
    {
        return false;
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

    if (!RuntimeContext)
    {
        return false;
    }

    const EBattlePhase CurrentPhase = BattleManager->GetCurrentPhase();
    
    if (CurrentPhase == EBattlePhase::CardSelect)//카드 선택 못한 상태에서 타임아웃 된 경우
    {
        //일반 카드 선택 요청과 동일하게 CardSelectTask를 완료
      
        if (!RequestPlayerCardSelection(PanicCard))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Failed to request panic card selection"));

            return false;
        }
    }
    else if (CurrentPhase == EBattlePhase::Targeting)//일반 카드 타게팅 도중 타임아웃된 경우 
    {
        //TargetingTask는 유지하고 기존 Player Session만 교체

        PlayerTargetingSession = nullptr;
        PlayerTargetingCard = PanicCard;

        if (TargetingPresentationController)
        {
            TargetingPresentationController->ClearCurrentStepPreview();
        }

        RuntimeContext->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());

        if (!StartPlayerTargeting())
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Failed to start panic targeting"));

            return false;
        }
    }
    else
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[BattleTargetingManager] "
                "Panic targeting rejected. Phase=%d"),
            static_cast<int32>(CurrentPhase));

        return false;
    }

    return CompletePlayerPanicTargeting();
}

bool ABattleTargetingManager::StartPlayerTargeting()
{
    if (!IsValid(PlayerTargetingCard))
        return false;

    ABattleGridManager* RuntimeGridManager = BattleManager->GetBattleGridManager();

    TargetingPresentationController->ClearStepPreviews();
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
    RefreshTargetingPresentation(PlayerTargetingSession);
    return true;
}

bool ABattleTargetingManager::RequestConfirmPlayerTargeting()
{
    if (!CanProcessPlayerTargetingInput())
    {
        return false;
    }

    const ETargetingConfirmResult ConfirmResult = PlayerTargetingSession->ConfirmStep();


    RefreshTargetingPresentation(PlayerTargetingSession);
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
        RefreshTargetingPresentation(PlayerTargetingSession);
        return true;
    }
    FHexOffsetCoord CandidateCoord = FHexOffsetCoord::Invalid();
    if (!GetGridCoordFromHit(HitResult, CandidateCoord))
    {
        PlayerTargetingSession->UpdateSelection(FHexOffsetCoord::Invalid(), INDEX_NONE);
        RefreshTargetingPresentation(PlayerTargetingSession);
        return false;
    }

    const int32 Direction = CalculateDirectionToCoord(PlayerTargetingSession, CandidateCoord);
    const bool bUpdated = PlayerTargetingSession->UpdateSelection(CandidateCoord, Direction);


    RefreshTargetingPresentation(PlayerTargetingSession);
    return bUpdated;
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

    RefreshTargetingPresentation(PlayerTargetingSession);

    BattleManager->GetBattleRuntimeContext()->ClearPlayerExchangeAction(BattleManager->GetCurrentExchange());

    UE_LOG(LogTemp, Log, TEXT("[BattleTargetingManager] Player targeting undo. RestoredStep=%d"),
        PlayerTargetingSession->GetCurrentStepIndex());
    return true;
}

void ABattleTargetingManager::CancelPlayerTargeting()
{
    PlayerTargetingSession = nullptr;
    TargetingPresentationController->ClearCurrentStepPreview();

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
    if (!PlayerTargetingSession || !PlayerTargetingSession->IsCompleted() || !IsValid(PlayerTargetingCard))
    {
        return false;
    }

    ABattleCharacterBase* PlayerCharacter = BattleManager->GetBattleRuntimeContext()->GetPlayerCharacter();
    if (!IsValid(PlayerCharacter))
    {
        return false;
    }

    if (!BattleManager->SubmitTargetingAction(
        PlayerCharacter,
        PlayerTargetingCard,
        PlayerTargetingSession->GetIntent(),
        true))
    {
        return false;
    }

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

bool ABattleTargetingManager::RequestEnemyPanicTargeting(UMuksiBattleCardDataAsset* PanicCard)
{
    if (!BattleManager || !IsValid(PanicCard))
    {
        return false;
    }

    const EBattlePhase CurrentPhase = BattleManager->GetCurrentPhase();

    if (CurrentPhase != EBattlePhase::CardSelect && CurrentPhase != EBattlePhase::Targeting)
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT(
                "[BattleTargetingManager] "
                "Enemy panic targeting rejected. Phase=%d"),
            static_cast<int32>(CurrentPhase));

        return false;
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

    ABattleGridManager* GridManager = BattleManager->GetBattleGridManager();

    ABattleSimulationManager* SimulationManager = BattleManager->GetBattleSimulationManager();

    if (!RuntimeContext || !GridManager || !SimulationManager)
    {
        return false;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();

    // 이미 EnemyAction이 완성됐다면 패닉 처리하지 않는다.
    if (RuntimeContext->GetEnemyExchangeAction(ExchangeIndex))
    {
        return true;
    }

    ABattleCharacter_Enemy* EnemyCharacter = RuntimeContext->GetEnemyCharacter();

    if (!IsValid(EnemyCharacter))
    {
        return false;
    }


    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);

    EnemyTargetingSession = nullptr;
    bEnemyCardPresentationFinished = false;

    ABattleCharacterBase* EnemyTargetingActor = SimulationManager->GetCharacterForWorld(EnemyCharacter, EnemyTargetingWorldType);

    if (!IsValid(EnemyTargetingActor))
    {
        return false;
    }

    EnemyTargetingSession = NewObject<UBattleTargetingSession>(this);

    if (!EnemyTargetingSession
        || !EnemyTargetingSession->StartSession(
            EnemyTargetingActor,
            GridManager,
            EnemyTargetingWorldType,
            PanicCard->TargetingData))
    {
        EnemyTargetingSession = nullptr;
        return false;
    }

    //Targeting Step이 있으면 타임아웃 전용 Strategy 사용

    if (EnemyTargetingSession->IsSelecting())
    {
        UClass* StrategyClass = PanicCard->PanicStrategyClass.Get();

        if (!StrategyClass || StrategyClass->HasAnyClassFlags(CLASS_Abstract))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Enemy PanicStrategyClass is invalid. "
                    "Card=%s"),
                *GetNameSafe(PanicCard));

            EnemyTargetingSession = nullptr;
            return false;
        }

        UPanicStrategyBase* PanicStrategy = NewObject<UPanicStrategyBase>(this, StrategyClass);

        if (!PanicStrategy)
        {
            EnemyTargetingSession = nullptr;
            return false;
        }

        FPanicStrategyContext Context;
        Context.SourceCharacter = EnemyTargetingActor;
        Context.BattleManager = BattleManager;
        Context.PanicCard = PanicCard;
        Context.WorldType = EnemyTargetingWorldType;

        while (EnemyTargetingSession->IsSelecting())
        {
            const int32 StepIndex = EnemyTargetingSession->GetCurrentStepIndex();

            // Enemy 패닉 좌표 결정
            const FPanicStrategyResult StrategyResult = PanicStrategy->SelectTarget(Context);

            if (!StrategyResult.IsValid())
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT(
                        "[BattleTargetingManager] "
                        "Enemy panic strategy returned "
                        "invalid target. Card=%s Step=%d"),
                    *GetNameSafe(PanicCard),
                    StepIndex);

                EnemyTargetingSession = nullptr;
                return false;
            }

            // 카드 TargetingData로 좌표 검증
            if (!EnemyTargetingSession->UpdateSelection(StrategyResult.TargetCoord, StrategyResult.Direction))
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT(
                        "[BattleTargetingManager] "
                        "Enemy panic target rejected. "
                        "Card=%s Target=(%d,%d)"),
                    *GetNameSafe(PanicCard),
                    StrategyResult.TargetCoord.X,
                    StrategyResult.TargetCoord.Y);

                EnemyTargetingSession = nullptr;
                return false;
            }

            if (EnemyTargetingSession->ConfirmStep() == ETargetingConfirmResult::Failed)
            {
                UE_LOG(
                    LogTemp,
                    Error,
                    TEXT(
                        "[BattleTargetingManager] "
                        "Enemy panic target confirm failed. "
                        "Card=%s Step=%d"),
                    *GetNameSafe(PanicCard),
                    StepIndex);

                EnemyTargetingSession = nullptr;
                return false;
            }
        }
    }

    if (!EnemyTargetingSession->IsCompleted())
    {
        EnemyTargetingSession = nullptr;
        return false;
    }

    //EnemyAction 생성
    if (!BattleManager->SubmitTargetingAction(
        EnemyCharacter,
        PanicCard,
        EnemyTargetingSession->GetIntent(),
        false))
    {
        EnemyTargetingSession = nullptr;
        return false;
    }
    
    UMuksiBattleCardDataAsset* PresentedCard = PanicCard;

    if (UMuksiBattleCardDataAsset* DeceivedCard = PanicCard->GetDeceivedCard())
    {
        PresentedCard = DeceivedCard;
    }

    OnEnemyCardSelectionReady.Broadcast(PresentedCard, ExchangeIndex);//위젯 표시
    return true;
}

void ABattleTargetingManager::CancelPendingEnemyCardSelection()
{
    const bool bWasTimerActive = GetWorldTimerManager().IsTimerActive(EnemyCardSelectionTimerHandle);

    GetWorldTimerManager().ClearTimer(EnemyCardSelectionTimerHandle);

    // 아직 완성되지 않은 Enemy Targeting 결과 제거
    EnemyTargetingSession = nullptr;
    bEnemyCardPresentationFinished = false;

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "[BattleTargetingManager] "
            "Pending enemy card selection cancelled. "
            "TimerActive=%s"),
        bWasTimerActive ? TEXT("true") : TEXT("false"));
}

void ABattleTargetingManager::CompleteEnemyCardSelectionRequest()
{
    UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
    ABattleCharacter_Enemy* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();
    if (!IsValid(EnemyCharacter))
    {
        return;
    }

    UMuksiBattleCardDataAsset* SelectedCard = nullptr;
    FTargetingIntent TargetingIntent;
    if (!CompleteEnemyTargeting(SelectedCard, TargetingIntent))
    {
        return;
    }

    if (!BattleManager->SubmitTargetingAction(EnemyCharacter, SelectedCard, TargetingIntent, false))
    {
        return;
    }

    UMuksiBattleCardDataAsset* PresentedCard = SelectedCard;
    if (UMuksiBattleCardDataAsset* DeceivedCard = SelectedCard->GetDeceivedCard())
    {
        PresentedCard = DeceivedCard;
    }

    OnEnemyCardSelectionReady.Broadcast(PresentedCard, BattleManager->GetCurrentExchange());
}

bool ABattleTargetingManager::CompleteEnemyTargeting(UMuksiBattleCardDataAsset*& OutSelectedCard, FTargetingIntent& OutIntent)
{
    OutSelectedCard = nullptr;
    OutIntent.Reset();

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


    TargetingPresentationController->ClearStepPreviews();
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

    OutSelectedCard = SelectedCard;
    OutIntent = EnemyTargetingSession->GetIntent();
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

        if (!EnemyTargetingSession->UpdateSelection(TargetCoord, Direction))
        {
            UE_LOG(LogTemp, Error,
                TEXT("[BattleTargetingManager] Enemy targeting selection failed. Step=%d Card=%s Target=(%d,%d) Direction=%d"),
                StepIndex,
                *GetNameSafe(SelectedCard),
                TargetCoord.X,
                TargetCoord.Y,
                Direction);
            return false;
        }

        if (EnemyTargetingSession->ConfirmStep() == ETargetingConfirmResult::Failed)
        {
            UE_LOG(LogTemp, Error,
                TEXT("[BattleTargetingManager] Enemy targeting confirm failed. Step=%d Card=%s Target=(%d,%d) Direction=%d"),
                StepIndex,
                *GetNameSafe(SelectedCard),
                TargetCoord.X,
                TargetCoord.Y,
                Direction);
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

    TargetingPresentationController->ClearCurrentStepPreview();
    ClearCardRevealPresentation();

    if (PlayerAction && PlayerTargetingSession && PlayerTargetingSession->IsCompleted())
        PresentCardReveal(PlayerTargetingSession);
    if (EnemyAction && EnemyTargetingSession && EnemyTargetingSession->IsCompleted())
        PresentCardReveal(EnemyTargetingSession);

    if (!PlayerAction && !EnemyAction)
    {
        FinishCardRevealPresentation();
        return;
    }

    GetWorldTimerManager().ClearTimer(CardRevealPreviewTimerHandle);
    GetWorldTimerManager().SetTimer(CardRevealPreviewTimerHandle, this,&ABattleTargetingManager::FinishCardRevealPresentation, FMath::Max(CardRevealPreviewDuration, 0.05f), false);
}

void ABattleTargetingManager::PresentCardReveal(const UBattleTargetingSession* Session)
{
    if (!Session || !Session->IsCompleted())
        return;

    ABattleGridManager* GridManager = BattleManager->GetBattleGridManager();
    ABattleCharacterBase* PreviewSource = Session->GetSourceCharacter();
    if (!IsValid(GridManager) || !IsValid(PreviewSource))
        return;

    const FTargetingCardData& TargetingData = Session->GetCardTargetingData();
    const TArray<FTargetingStepResult>& ConfirmedSteps = Session->GetConfirmedSteps();
    const int32 StepCount = FMath::Min(TargetingData.Steps.Num(), ConfirmedSteps.Num());

    for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
    {
        const FTargetingStepCardData* StepData = TargetingData.GetStep(StepIndex);
        if (!StepData)
            continue;

        const FTargetingPhasePresentationSettings& PresentationSettings = StepData->Presentation.Phases.CardReveal;
        if (!PresentationSettings.HasAnyPresentation())
            continue;

        const FTargetingStepResult& TargetingStep = ConfirmedSteps[StepIndex];
        if (!TargetingStep.Step.HasOriginCoord() || !TargetingStep.Step.HasTargetCoord())
            continue;

        FTargetingPreviewContext PreviewContext;
        PreviewContext.SourceCharacter = PreviewSource;
        PreviewContext.GridManager = GridManager;
        PreviewContext.StepData = StepData;
        PreviewContext.TargetingStep = &TargetingStep;
        PreviewContext.PresentationSettings = &PresentationSettings;

        TargetingPresentationController->AddStepPreview(PreviewContext);
    }
}

void ABattleTargetingManager::ClearCardRevealPresentation()
{
    TargetingPresentationController->ClearStepPreviews();
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

bool ABattleTargetingManager::CompletePlayerPanicTargeting()
{
    if (!BattleManager || !PlayerTargetingSession || !IsValid(PlayerTargetingCard))
    {
        return false;
    }

    UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

    if (!RuntimeContext)
    {
        return false;
    }

    const int32 ExchangeIndex = BattleManager->GetCurrentExchange();

    //Targeting Step이 없는 카드 ex)그냥 방어, 자체 디버프 부여 같은 아무것도 안하는 카드
    //바로 Completed
    if (PlayerTargetingSession->IsCompleted())
    {
       
        if (RuntimeContext->GetPlayerExchangeAction(ExchangeIndex))
        {
            // RequestPlayerCardSelection() 안에서
            // 이미 Action을 만들졌으면 그거 보내기
            return true;
        }

        return CompletePlayerTargeting();
    }


    //Targeting Step이 존재하는데 Strategy가 없다면 Error
    UClass* StrategyClass = PlayerTargetingCard->PanicStrategyClass.Get();
    if (!StrategyClass || StrategyClass->HasAnyClassFlags(CLASS_Abstract))
    {
        UE_LOG(LogTemp, Error, TEXT("PanicStrategyClass is invalid. Card=%s (BattleTargetingManager.cpp)"), *GetNameSafe(PlayerTargetingCard));
        return false;
    }
    
    UPanicStrategyBase* PanicStrategy = NewObject<UPanicStrategyBase>(this, StrategyClass);
    if (!PanicStrategy)
    {
        return false;
    }

    FPanicStrategyContext Context;
    Context.SourceCharacter = PlayerTargetingSession->GetSourceCharacter();
    Context.BattleManager = BattleManager;
    Context.PanicCard = PlayerTargetingCard;
    Context.WorldType = PlayerTargetingWorldType;

    while (PlayerTargetingSession->IsSelecting())
    {
        const int32 StepIndex = PlayerTargetingSession->GetCurrentStepIndex();

        // 실제 좌표를 결정하는 부분
        const FPanicStrategyResult StrategyResult = PanicStrategy->SelectTarget(Context);

        if (!StrategyResult.IsValid())
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Panic strategy returned invalid target. "
                    "Card=%s Step=%d"),
                *GetNameSafe(PlayerTargetingCard),
                StepIndex);

            return false;
        }

        // 카드 TargetingData의 Selection Rule로 좌표 검증
        if (!PlayerTargetingSession->UpdateSelection(
            StrategyResult.TargetCoord,
            StrategyResult.Direction))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Panic target rejected. "
                    "Card=%s Step=%d Target=(%d,%d)"),
                *GetNameSafe(PlayerTargetingCard),
                StepIndex,
                StrategyResult.TargetCoord.X,
                StrategyResult.TargetCoord.Y);

            return false;
        }

        const ETargetingConfirmResult ConfirmResult = PlayerTargetingSession->ConfirmStep();

        if (ConfirmResult == ETargetingConfirmResult::Failed)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT(
                    "[BattleTargetingManager] "
                    "Panic target confirm failed. "
                    "Card=%s Step=%d"),
                *GetNameSafe(PlayerTargetingCard),
                StepIndex);

            return false;
        }
    }

    if (!PlayerTargetingSession->IsCompleted())
    {
        return false;
    }

    // FBattleAction 생성
    return CompletePlayerTargeting();
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

bool ABattleTargetingManager::GetGridCoordFromHit(const FHitResult& HitResult, FHexOffsetCoord& OutCoord) const
{
    OutCoord = FHexOffsetCoord::Invalid();

    if (ABattleCharacterBase* HitCharacter = Cast<ABattleCharacterBase>(HitResult.GetActor()))
    {
        ABattleCharacterBase* RuntimeCharacter = BattleManager->GetBattleSimulationManager()->GetCharacterForWorld(HitCharacter, PlayerTargetingWorldType);
        OutCoord = RuntimeCharacter ? RuntimeCharacter->GetCharacterCoord() : FHexOffsetCoord::Invalid();
        return OutCoord.IsValid();
    }

    ABattleGridManager* RuntimeGridManager = BattleManager->GetBattleGridManager();
    return RuntimeGridManager && RuntimeGridManager->GetPresentationCoordFromHit(HitResult, OutCoord);
}


