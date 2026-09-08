#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

ABattleSequenceManager::ABattleSequenceManager() { PrimaryActorTick.bCanEverTick = false; }
void ABattleSequenceManager::BeginPlay() { Super::BeginPlay(); }

void ABattleSequenceManager::EndPlay(const EEndPlayReason::Type Reason)
{
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);
	if (BattleManager)
		BattleManager->PhaseExecutionRequestedDelegate.RemoveDynamic(this, &ABattleSequenceManager::HandlePhaseExecutionRequested);
	ClearBattleActionPresentation();
	ResetBattleActionSequence();
	if (ActionExecutor)
	{
		ActionExecutor->OnBattleActionStarted.Unbind();
		ActionExecutor->OnBattleActionCompleted.Unbind();
		ActionExecutor->OnExecutionEntryStarted.Unbind();
		ActionExecutor->Stop();
	}
	ActionExecutor = nullptr;
	PhaseExecutionTask = nullptr;
	BattleManager = nullptr;
	BattleRuntimeContext = nullptr;
	Super::EndPlay(Reason);
}

bool ABattleSequenceManager::InitializeBattleFlow(ABattleManager* InManager, UBattleRuntimeContext* InContext, ABattleGridManager* InGrid)
{
	if (!IsValid(InManager) || !IsValid(InContext) || !IsValid(InGrid))
		return false;
	BattleManager = InManager;
	BattleRuntimeContext = InContext;
	BattleGridManager = InGrid;
	BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleSequenceManager::HandlePhaseExecutionRequested);
	ActionExecutor = NewObject<UBattleActionExecutor>(this);
	if (!ActionExecutor)
		return false;
	if (!ActionExecutor->Initialize(BattleGridManager, EBattleSimulationWorldType::PlayerActualEnemyActual))
		return false;
	ActionExecutor->OnBattleActionStarted.BindUObject(this, &ABattleSequenceManager::HandleBattleActionStarted);
	ActionExecutor->OnBattleActionCompleted.BindUObject(this, &ABattleSequenceManager::HandleBattleActionCompleted);
	ActionExecutor->OnExecutionEntryStarted.BindUObject(this, &ABattleSequenceManager::HandleExecutionEntryStarted);
	return true;
}

void ABattleSequenceManager::InitializeBattleRuntimeContext(UBattleRuntimeContext* InContext) { BattleRuntimeContext = InContext; }

void ABattleSequenceManager::HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	(void)OldPhase;
	if (NewPhase != EBattlePhase::BattleActionSequenceStart || !TaskContext)
		return;
	PhaseExecutionTask = TaskContext->RegisterTask(this);
	if (PhaseExecutionTask)
		ExecuteBattleActionSequence();
}

void ABattleSequenceManager::ExecuteBattleActionSequence()
{
	if (!IsValid(BattleRuntimeContext) && IsValid(BattleManager))
		InitializeBattleRuntimeContext(BattleManager->GetBattleRuntimeContext());
	if (!IsValid(BattleRuntimeContext))
	{
		CompleteBattleActionSequencePhase();
		return;
	}
	const TArray<FBattleAction>& BattleActions = BattleRuntimeContext->GetBattleActionSequenceQueue();
	if (BattleActions.IsEmpty() || !StartBattleActionSequence(BattleActions))
		CompleteBattleActionSequencePhase();
}

void ABattleSequenceManager::CompleteBattleActionSequencePhase()
{
	if (IsValid(BattleManager) && BattleManager->GetCurrentPhase() != EBattlePhase::BattleActionSequenceStart)
	{
		PhaseExecutionTask = nullptr;
		return;
	}
	UBattlePhaseTask* CompletedTask = PhaseExecutionTask;
	PhaseExecutionTask = nullptr;
	if (CompletedTask)
		CompletedTask->Complete();
}

bool ABattleSequenceManager::IsBattleActionRunning() const { return ActionExecutor && ActionExecutor->IsRunning(); }

bool ABattleSequenceManager::StartBattleActionSequence(const TArray<FBattleAction>& InBattleActions)
{
	if (bBattleActionSequenceRunning || IsBattleActionRunning() || InBattleActions.IsEmpty())
		return false;
	BattleActionQueue = InBattleActions;
	SortBattleActionQueue();
	CurrentBattleActionIndex = 0;
	bBattleActionSequenceRunning = true;
	bBattleActionCompletionPending = false;
	bStopAfterCurrentExecution = false;
	StartCurrentBattleAction();
	return true;
}

void ABattleSequenceManager::SortBattleActionQueue()
{
	BattleActionQueue.Sort([](const FBattleAction& A, const FBattleAction& B)
	{
		if (A.ExchangeIndex != B.ExchangeIndex)
			return A.ExchangeIndex < B.ExchangeIndex;
		if (A.Speed != B.Speed)
			return A.Speed > B.Speed;
		if (A.bPlayerAction != B.bPlayerAction)
			return A.bPlayerAction;
		return false;
	});
}

void ABattleSequenceManager::StartCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || bWaitingForDeceiveCardReveal)
		return;
	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		FinishBattleActionSequence();
		return;
	}
	const FBattleAction& Action = BattleActionQueue[CurrentBattleActionIndex];
	if (!IsValid(Action.Attacker.Get()) || !IsValid(Action.Card.Get()))
	{
		FinishCurrentBattleAction();
		return;
	}
	if (ShouldRequestDeceiveCardReveal(Action) && DeceiveCardRevealRequestedDelegate.IsBound())
	{
		bWaitingForDeceiveCardReveal = true;
		DeceiveCardRevealRequestedDelegate.Broadcast(Action);
		return;
	}
	ExecuteCurrentBattleAction();
}

bool ABattleSequenceManager::ShouldRequestDeceiveCardReveal(const FBattleAction& Action) const
{
	return IsValid(Action.Card.Get()) && IsValid(Action.Card->GetActualCard());
}

void ABattleSequenceManager::StopAfterCurrentExecution()
{
	if (!bBattleActionSequenceRunning || bStopAfterCurrentExecution)
		return;

	bStopAfterCurrentExecution = true;
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);

	if (BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
		BattleActionQueue.SetNum(CurrentBattleActionIndex + 1);

	if (ActionExecutor)
		ActionExecutor->StopAfterCurrentExecution();
}

void ABattleSequenceManager::NotifyDeceiveCardRevealFinished()
{
	if (!bBattleActionSequenceRunning || !bWaitingForDeceiveCardReveal)
		return;
	bWaitingForDeceiveCardReveal = false;
	ExecuteCurrentBattleAction();
}

void ABattleSequenceManager::ExecuteCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || !BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
		return;
	const FBattleAction& Action = BattleActionQueue[CurrentBattleActionIndex];
	if (!ActionExecutor)
	{
		FinishCurrentBattleAction();
		return;
	}

	if (!ActionExecutor->ExecuteBattleAction(Action))
		FinishCurrentBattleAction();
}

void ABattleSequenceManager::HandleBattleActionStarted(const FBattleAction& Action)
{
	BattleActionStartedDelegate.Broadcast(Action);
}

void ABattleSequenceManager::HandleExecutionEntryStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FTargetingResult& TargetingResult)
{
	if (bBattleActionSequenceRunning)
		PresentBattleActionTargetingResult(Action, TargetingResult);
}

void ABattleSequenceManager::HandleBattleActionCompleted()
{
	if (BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
		BattleActionCompletedDelegate.Broadcast(BattleActionQueue[CurrentBattleActionIndex]);

	if (bBattleActionSequenceRunning)
		FinishCurrentBattleAction();
}

void ABattleSequenceManager::FinishCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || bBattleActionCompletionPending)
		return;
	bBattleActionCompletionPending = true;
	ClearBattleActionPresentation();
	++CurrentBattleActionIndex;
	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		FinishBattleActionSequence();
		return;
	}
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);
	NextBattleActionTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ABattleSequenceManager::StartNextBattleActionDeferred));
}

void ABattleSequenceManager::StartNextBattleActionDeferred()
{
	bBattleActionCompletionPending = false;
	StartCurrentBattleAction();
}

void ABattleSequenceManager::FinishBattleActionSequence()
{
	if (!bBattleActionSequenceRunning)
		return;
	ResetBattleActionSequence();
	CompleteBattleActionSequencePhase();
}

void ABattleSequenceManager::ResetBattleActionSequence()
{
	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);
	BattleActionQueue.Empty();
	CurrentBattleActionIndex = INDEX_NONE;
	bBattleActionSequenceRunning = false;
	bBattleActionCompletionPending = false;
	bWaitingForDeceiveCardReveal = false;
	bStopAfterCurrentExecution = false;
}

void ABattleSequenceManager::PresentBattleActionTargetingResult(const FBattleAction& Action, const FTargetingResult& TargetingResult)
{
	ClearBattleActionPresentation();
	if (!IsValid(BattleGridManager) || !IsValid(Action.Card.Get()) || !IsValid(BattleManager.Get()))
		return;

	ABattleTargetingManager* TargetingManager = BattleManager->GetBattleTargetingManager();
	UTargetingPresentationController* PresentationController = TargetingManager ? TargetingManager->GetPresentationController() : nullptr;
	if (!PresentationController)
		return;

	for (int32 StepIndex = 0; StepIndex < Action.Card->TargetingData.Steps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
		const FTargetingStepResult* StepResult = TargetingResult.GetStep(StepIndex);
		if (!StepData || !StepResult)
			continue;
		const FTargetingPhasePresentationSettings& Settings = StepData->Presentation.Phases.ActualBattle;
		if (!Settings.HasAnyPresentation())
			continue;

		FTargetingPreviewContext PreviewContext;
		PreviewContext.SourceCharacter = Action.Attacker.Get();
		PreviewContext.GridManager = BattleGridManager.Get();
		PreviewContext.StepData = StepData;
		PreviewContext.TargetingStep = StepResult;
		PreviewContext.PresentationSettings = &Settings;
		PresentationController->AddStepPreview(PreviewContext);
	}
}

void ABattleSequenceManager::ClearBattleActionPresentation()
{
	if (!IsValid(BattleManager.Get()))
		return;
	ABattleTargetingManager* TargetingManager = BattleManager->GetBattleTargetingManager();
	UTargetingPresentationController* PresentationController = TargetingManager ? TargetingManager->GetPresentationController() : nullptr;
	if (PresentationController)
		PresentationController->ClearStepPreviews();
}
