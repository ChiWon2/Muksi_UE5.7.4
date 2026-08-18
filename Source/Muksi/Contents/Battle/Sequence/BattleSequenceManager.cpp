#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Sequence/Environment/BattleSequenceExecutionEnvironment.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"

// ============================================================================
// 생명주기
// ============================================================================
ABattleSequenceManager::ABattleSequenceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleSequenceManager::BeginPlay()
{
	Super::BeginPlay();
}

void ABattleSequenceManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);

	if (BattleManager)
	{
		BattleActionStartDelegate.RemoveAll(BattleManager);
		BattleManager->PhaseExecutionRequestedDelegate.RemoveDynamic(this, &ABattleSequenceManager::HandlePhaseExecutionRequested);
	}
	PhaseExecutionTask = nullptr;

	ClearBattleActionPresentation();
	ResetBattleActionSequence();
	BattleManager = nullptr;
	BattleRuntimeContext = nullptr;

	Super::EndPlay(EndPlayReason);
}

bool ABattleSequenceManager::InitializeBattleFlow(ABattleManager* InBattleManager, UBattleRuntimeContext* InBattleRuntimeContext, ABattleGridManager* InBattleGridManager)
{
	if (!IsValid(InBattleManager) || !IsValid(InBattleRuntimeContext) || !IsValid(InBattleGridManager))
		return false;

	BattleManager = InBattleManager;
	BattleRuntimeContext = InBattleRuntimeContext;
	BattleGridManager = InBattleGridManager;
	BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleSequenceManager::HandlePhaseExecutionRequested);

	if (!TargetingPresentationController)
		TargetingPresentationController = NewObject<UTargetingPresentationController>(this);

	if (!TargetingPresentationController)
		return false;

	TargetingPresentationController->Initialize(BattleGridManager);

	BattleActionStartDelegate.AddUObject(BattleManager, &ABattleManager::NotifyBattleActionStart);
	return true;
}

void ABattleSequenceManager::InitializeBattleRuntimeContext(UBattleRuntimeContext* InBattleRuntimeContext)
{
	BattleRuntimeContext = InBattleRuntimeContext;
}

void ABattleSequenceManager::HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	(void)OldPhase;
	if (NewPhase != EBattlePhase::BattleActionSequenceStart || !TaskContext) return;
	PhaseExecutionTask = TaskContext->RegisterTask(this);
	if (!PhaseExecutionTask) return;
	ExecuteBattleActionSequence();
}

void ABattleSequenceManager::ExecuteBattleActionSequence()
{

	if (!IsValid(BattleRuntimeContext) && IsValid(BattleManager))
	{
		InitializeBattleRuntimeContext(BattleManager->GetBattleRuntimeContext());
	}

	if (!IsValid(BattleRuntimeContext))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSequenceManager] BattleRuntimeContext is invalid."));
		NotifyBattleActionSequenceCompleted();
		return;
	}

	const TArray<FBattleAction>& SequenceActions = BattleRuntimeContext->GetBattleActionSequenceQueue();
	if (SequenceActions.IsEmpty() || !StartBattleActionSequence(SequenceActions))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] Battle Action Sequence could not start."));
		NotifyBattleActionSequenceCompleted();
	}
}

void ABattleSequenceManager::NotifyBattleActionSequenceCompleted()
{
	if (IsValid(BattleManager) && BattleManager->GetCurrentPhase() != EBattlePhase::BattleActionSequenceStart)
	{
		PhaseExecutionTask = nullptr;
		return;
	}
	UBattlePhaseTask* CompletedTask = PhaseExecutionTask;
	PhaseExecutionTask = nullptr;
	if (CompletedTask) CompletedTask->Complete();
}

// ============================================================================
// Sequence 진입
// BattleRuntimeContext 또는 내부 Action Queue -> StartSequence(WithRequest)
// ============================================================================
bool ABattleSequenceManager::StartSequence(const FBattleAction& InAction)
{
	FBattleSequenceRequest Request;
	Request.Action = InAction;
	Request.ExecutionMode = EBattleExecutionMode::Sequence;
	return StartSequenceWithRequest(Request);
}

bool ABattleSequenceManager::StartSequenceWithRequest(const FBattleSequenceRequest& Request)
{
	if (bSequenceRunning || (bBattleActionSequenceRunning && !bStartingQueuedBattleAction) || !ValidateRequest(Request))
	{
		return false;
	}

	FBattleAction SequenceAction = Request.Action;
	UMuksiBattleCardDataAsset* ExecutionCard = Request.GetExecutionCard();

	if (!FBattleTargetResolver::ResolveAction(SequenceAction, BattleGridManager, CurrentResolvedTargeting))
	{
		return false;
	}

	CurrentAction = MoveTemp(SequenceAction);
	CurrentExecutionCard = ExecutionCard;
	CurrentExecutionMode = Request.ExecutionMode;
	bSequenceRunning = true;
	ActiveExecutionRunners.Empty();

	if (!InitializeExecutionEnvironment())
	{
		FinishSequence();
		return false;
	}

	if (!BindAttackerNotify())
	{
		FinishSequence();
		return false;
	}

	BattleActionStartDelegate.Broadcast(CurrentAction);
	StartMainExecutionChain();
	return true;
}

// ============================================================================
// Battle Action Queue 실행
// 정렬/인덱스/다음 틱 진행은 이 Manager가 전담한다.
// ============================================================================
bool ABattleSequenceManager::StartBattleActionSequence(const TArray<FBattleAction>& InActions)
{
	if (bBattleActionSequenceRunning || bSequenceRunning || InActions.IsEmpty())
	{
		return false;
	}

	BattleActionQueue = InActions;
	SortBattleActionQueue();
	CurrentBattleActionIndex = 0;
	bBattleActionSequenceRunning = true;
	bBattleActionCompletionPending = false;

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Battle Action Sequence Started. ActionCount=%d"), BattleActionQueue.Num());
	StartCurrentBattleAction();
	return true;
}

void ABattleSequenceManager::SortBattleActionQueue()
{
	BattleActionQueue.Sort([](const FBattleAction& A, const FBattleAction& B)
	{
		if (A.ExchangeIndex != B.ExchangeIndex)
		{
			return A.ExchangeIndex < B.ExchangeIndex;
		}

		if (A.Speed != B.Speed)
		{
			return A.Speed > B.Speed;
		}

		if (A.bPlayerAction != B.bPlayerAction)
		{
			return A.bPlayerAction;
		}

		return false;
	});
}

void ABattleSequenceManager::StartCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || bWaitingForDeceiveCardReveal)
	{
		return;
	}

	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		FinishBattleActionSequence();
		return;
	}

	const FBattleAction& CurrentQueuedAction = BattleActionQueue[CurrentBattleActionIndex];
	if (!IsValid(CurrentQueuedAction.Attacker) || !IsValid(CurrentQueuedAction.Card))
	{
		HandleCurrentBattleActionFinished();
		return;
	}

	if (ShouldRequestDeceiveCardReveal(CurrentQueuedAction) && DeceiveCardRevealRequestedDelegate.IsBound())
	{
		bWaitingForDeceiveCardReveal = true;
		DeceiveCardRevealRequestedDelegate.Broadcast(CurrentQueuedAction);
		return;
	}

	StartCurrentBattleActionExecution();
}

bool ABattleSequenceManager::ShouldRequestDeceiveCardReveal(const FBattleAction& Action) const
{
	if (!IsValid(Action.Card))
	{
		return false;
	}

	return IsValid(Action.Card->GetDeceivedCard());
}

void ABattleSequenceManager::NotifyDeceiveCardRevealFinished()
{
	if (!bBattleActionSequenceRunning || !bWaitingForDeceiveCardReveal)
	{
		return;
	}

	bWaitingForDeceiveCardReveal = false;
	StartCurrentBattleActionExecution();
}

void ABattleSequenceManager::StartCurrentBattleActionExecution()
{
	if (!bBattleActionSequenceRunning || !BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		return;
	}

	const FBattleAction& CurrentQueuedAction = BattleActionQueue[CurrentBattleActionIndex];
	bStartingQueuedBattleAction = true;
	const bool bStarted = StartSequence(CurrentQueuedAction);
	bStartingQueuedBattleAction = false;

	if (!bStarted)
	{
		HandleCurrentBattleActionFinished();
	}
}

void ABattleSequenceManager::HandleCurrentBattleActionFinished()
{
	if (!bBattleActionSequenceRunning || bBattleActionCompletionPending)
	{
		return;
	}

	bBattleActionCompletionPending = true;

	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] Invalid Battle Action index: %d"), CurrentBattleActionIndex);
		FinishBattleActionSequence();
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Battle Action Finished. Index=%d"), CurrentBattleActionIndex);
	ClearBattleActionPresentation();
	OnBattleActionFinished.Broadcast();
	FinishCurrentBattleAction();
}

void ABattleSequenceManager::FinishCurrentBattleAction()
{
	++CurrentBattleActionIndex;

	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		FinishBattleActionSequence();
		return;
	}

	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);
	NextBattleActionTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
		FTimerDelegate::CreateUObject(this, &ABattleSequenceManager::StartNextBattleActionDeferred));
}

void ABattleSequenceManager::StartNextBattleActionDeferred()
{
	bBattleActionCompletionPending = false;
	StartCurrentBattleAction();
}

void ABattleSequenceManager::FinishBattleActionSequence()
{
	if (!bBattleActionSequenceRunning)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Battle Action Sequence Finished."));
	ClearBattleActionPresentation();
	ResetBattleActionSequence();
	OnBattleActionSequenceFinished.Broadcast();
	NotifyBattleActionSequenceCompleted();
}

void ABattleSequenceManager::ResetBattleActionSequence()
{
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);
	BattleActionQueue.Empty();
	CurrentBattleActionIndex = INDEX_NONE;
	bBattleActionSequenceRunning = false;
	bBattleActionCompletionPending = false;
	bStartingQueuedBattleAction = false;
	bWaitingForDeceiveCardReveal = false;
}

void ABattleSequenceManager::RefreshBattleActionTargetingPresentation(
	const FBattleAction& Action,
	const FResolvedTargeting& ExecutionResolvedTargeting)
{
	ClearBattleActionPresentation();

	if (!IsValid(BattleGridManager) || !IsValid(Action.Card))
	{
		return;
	}

	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData)
		{
			continue;
		}

		FResolvedTargeting StepResolvedTargeting;
		if (StepIndex == StepCount - 1)
		{
			StepResolvedTargeting = ExecutionResolvedTargeting;
		}
		else if (!FBattleTargetResolver::ResolveActionThroughStep(
			Action,
			BattleGridManager,
			StepIndex,
			StepResolvedTargeting))
		{
			UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] Targeting preview resolve failed. Step=%d"), StepIndex);
			continue;
		}

		const FTargetingPhasePresentationSettings& PresentationSettings =
			StepData->AdvancedSettings.Presentation.AttackSequencePhase;

		if (PresentationSettings.bShowIndicator)
		{
			TArray<FHexOffsetCoord> StepIndicatorCoords = StepResolvedTargeting.AffectedCoords;
			if (StepIndicatorCoords.IsEmpty())
			{
				if (const FTargetingStepResult* StepResult = StepResolvedTargeting.GetStep(StepIndex))
				{
					if (StepResult->HasSelectedCoord())
					{
						StepIndicatorCoords.Add(StepResult->SelectedCoord);
					}
				}
			}

			for (const FHexOffsetCoord& Coord : StepIndicatorCoords)
			{
				IndicatorCoords.AddUnique(Coord);
			}
		}

		const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview
			|| PresentationSettings.bShowPathPreview
			|| PresentationSettings.bShowAreaPreview;
		if (bShowAnyPreview && TargetingPresentationController)
		{
			TargetingPresentationController->AddResolvedStepPreview(
				Action.Attacker,
				Action.Card->TargetingData,
				StepResolvedTargeting,
				StepIndex,
				PresentationSettings,
				!Action.bPlayerAction);
		}
	}

	if (!IndicatorCoords.IsEmpty())
	{
		BattleGridManager->SetExchangeIndicator(
			Action.Card->CardTypeInfo,
			IndicatorCoords,
			!Action.bPlayerAction);
	}
}

void ABattleSequenceManager::ClearBattleActionPresentation()
{
	if (TargetingPresentationController)
	{
		TargetingPresentationController->ClearExecutionPreview();
	}

	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
}

// ============================================================================
// 요청 검증 및 Execution 환경 준비
// ============================================================================
bool ABattleSequenceManager::ValidateRequest(const FBattleSequenceRequest& Request) const
{
	UMuksiBattleCardDataAsset* ExecutionCard = Request.GetExecutionCard();
	return IsValid(Request.Action.Attacker) && IsValid(Request.Action.Card) && IsValid(ExecutionCard) && !ExecutionCard->MainExecutions.IsEmpty();
}

bool ABattleSequenceManager::InitializeExecutionEnvironment()
{
	ExecutionEnvironment = NewObject<UBattleSequenceExecutionEnvironment>(this);

	if (!ExecutionEnvironment)
	{
		return false;
	}

	ExecutionEnvironment->InitializeSequence(CurrentAction.Attacker, CurrentExecutionCard, BattleGridManager);
	return ExecutionEnvironment->IsValidEnvironment();
}

bool ABattleSequenceManager::BindAttackerNotify()
{
	if (!CurrentExecutionCard || CurrentExecutionCard->NotifyExecutionChains.IsEmpty())
	{
		return true;
	}

	if (!CurrentAction.Attacker)
	{
		return false;
	}

	AttackerAnimationComponent = CurrentAction.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AttackerAnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] AnimationComponent not found. Attacker=%s"), *GetNameSafe(CurrentAction.Attacker.Get()));
		return false;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.AddUniqueDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
	return true;
}

void ABattleSequenceManager::UnbindAttackerNotify()
{
	if (!AttackerAnimationComponent)
	{
		return;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.RemoveDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
}

// ============================================================================
// Execution Chain 실행
// Main chain + Animation Notify chain + Runtime requested chain
// ============================================================================
void ABattleSequenceManager::StartMainExecutionChain()
{
	if (!CurrentExecutionCard || CurrentExecutionCard->MainExecutions.IsEmpty())
	{
		FinishSequence();
		return;
	}

	TArray<FBattleExecutionEntry> MainExecutions;

	if (CurrentExecutionMode == EBattleExecutionMode::Sequence && CurrentAction.Attacker)
	{
		if (UMuksiStatusEffectComponent* StatusEffectComponent = CurrentAction.Attacker->GetStatusEffectComponent())
		{
			StatusEffectComponent->AppendBattleActionStartExecutions(CurrentAction, MainExecutions);
		}
	}

	MainExecutions.Append(CurrentExecutionCard->MainExecutions);
	StartExecutionRunner(MainExecutions, MakeExecutionContext(NAME_None));
}

void ABattleSequenceManager::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (!bSequenceRunning || NotifyKey.IsNone())
	{
		return;
	}

	StartNotifyExecutionChains(NotifyKey);
}

void ABattleSequenceManager::StartNotifyExecutionChains(FName NotifyKey)
{
	if (!CurrentExecutionCard)
	{
		return;
	}

	for (const FBattleNotifyExecutionChain& NotifyChain : CurrentExecutionCard->NotifyExecutionChains)
	{
		if (!NotifyChain.IsValid() || NotifyChain.NotifyKey != NotifyKey)
		{
			continue;
		}

		StartExecutionRunner(NotifyChain.Executions, MakeExecutionContext(NotifyKey));
	}
}

void ABattleSequenceManager::StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	if (!bSequenceRunning || ExecutionEntries.IsEmpty())
	{
		return;
	}

	UBattleExecutionRunner* ExecutionRunner = NewObject<UBattleExecutionRunner>(this);

	if (!ExecutionRunner)
	{
		return;
	}

	ActiveExecutionRunners.Add(ExecutionRunner);

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionRunner Started. Runner=%s ActiveRunners=%d"), *GetNameSafe(ExecutionRunner), ActiveExecutionRunners.Num());

	FBattleExecutionEntryStarted OnEntryStarted;
	OnEntryStarted.BindUObject(this, &ABattleSequenceManager::HandleExecutionEntryStarted);

	FBattleExecutionEntryFinished OnEntryFinished;
	OnEntryFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionEntryFinished);

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionRunnerFinished);

	ExecutionRunner->Run(ExecutionEntries, Context, OnEntryStarted, OnEntryFinished, OnFinished);
}

FBattleExecutionContext ABattleSequenceManager::MakeExecutionContext(FName NotifyKey)
{
	FBattleExecutionContext Context;

	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentExecutionCard;
	Context.ExecutionMode = CurrentExecutionMode;
	Context.Environment = ExecutionEnvironment;
	Context.ResolvedTargeting = CurrentResolvedTargeting;
	Context.BattleGridManager = BattleGridManager;
	Context.NotifyKey = NotifyKey;

	return Context;
}

void ABattleSequenceManager::HandleExecutionEntryStarted(
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	FBattleExecutionContext& InOutExecutionContext)
{
	if (!bSequenceRunning)
	{
		return;
	}

	// Targeting intent is preserved for the whole sequence, while the resolved
	// result is rebuilt immediately before every execution against the current grid.
	// Preview and execution therefore consume the same snapshot.
	FResolvedTargeting RefreshedTargeting;
	if (FBattleTargetResolver::ResolveAction(CurrentAction, BattleGridManager, RefreshedTargeting))
	{
		CurrentResolvedTargeting = MoveTemp(RefreshedTargeting);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[BattleSequenceManager] Targeting refresh failed before execution. EntryIndex=%d Execution=%s"),
			EntryIndex,
			*GetNameSafe(Entry.ExecutionClass.Get()));
		CurrentResolvedTargeting.Reset();
	}

	InOutExecutionContext.ResolvedTargeting = CurrentResolvedTargeting;

	if (bBattleActionSequenceRunning)
	{
		RefreshBattleActionTargetingPresentation(CurrentAction, CurrentResolvedTargeting);
	}

	OnExecutionEntryStarted.Broadcast(CurrentAction, Entry, EntryIndex, CurrentResolvedTargeting);
}

void ABattleSequenceManager::HandleExecutionEntryFinished(
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	const FBattleExecutionContext& ExecutionContext)
{
	if (!bSequenceRunning)
	{
		return;
	}

	OnExecutionEntryFinished.Broadcast(CurrentAction, Entry, EntryIndex, ExecutionContext.ResolvedTargeting);
}

void ABattleSequenceManager::HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bSequenceRunning || !FinishedRunner)
	{
		return;
	}

	const int32 RemovedCount = ActiveExecutionRunners.RemoveSingle(FinishedRunner);

	if (RemovedCount == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionRunner Finished. Runner=%s ActiveRunners=%d"), *GetNameSafe(FinishedRunner), ActiveExecutionRunners.Num());

	TryFinishSequence();
}

// ============================================================================
// Sequence 종료
// 모든 Runner 완료 -> FinishSequence -> OnSequenceFinished
// ============================================================================
void ABattleSequenceManager::TryFinishSequence()
{
	if (!bSequenceRunning || !ActiveExecutionRunners.IsEmpty())
	{
		return;
	}

	FinishSequence();
}

void ABattleSequenceManager::FinishSequence()
{
	if (!bSequenceRunning)
	{
		return;
	}

	UnbindAttackerNotify();

	bSequenceRunning = false;
	CurrentAction = FBattleAction();
	CurrentExecutionCard = nullptr;
	CurrentResolvedTargeting.Reset();
	CurrentExecutionMode = EBattleExecutionMode::Sequence;
	AttackerAnimationComponent = nullptr;
	ActiveExecutionRunners.Empty();
	ExecutionEnvironment = nullptr;

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Sequence Finished."));

	OnSequenceFinished.Broadcast();

	if (bBattleActionSequenceRunning)
	{
		HandleCurrentBattleActionFinished();
	}
}
