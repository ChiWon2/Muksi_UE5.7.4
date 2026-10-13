#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/RuntimeModifier/BattleActionRuntimeModifier.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"
#include "Presentation/BattleActionGroupController.h"
#include "Presentation/BattleActionPresenter.h"

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
		ActionExecutor->OnBattleActionCompleted.Unbind();
		ActionExecutor->OnExecutionEntryStarted.Unbind();
		ActionExecutor->Stop();
	}
	ActionExecutor = nullptr;
	PhaseExecutionTask = nullptr;
	BattleManager = nullptr;
	BattleRuntimeContext = nullptr;
	
	if (IsValid(ActionPresenter))
	{
		ActionPresenter->Shutdown();
		ActionPresenter = nullptr;
	}
	
	if (IsValid(ActionGroupController))
	{
		ActionGroupController->Shutdown();
		ActionGroupController = nullptr;
	}
	
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
	ActionExecutor->OnBattleActionCompleted.BindUObject(this, &ABattleSequenceManager::HandleBattleActionCompleted);
	ActionExecutor->OnExecutionEntryStarted.BindUObject(this, &ABattleSequenceManager::HandleExecutionEntryStarted);
	
	ActionPresenter = NewObject<UBattleActionPresenter>(this);
	if (!IsValid(ActionPresenter))
		return false;
	if (!ActionPresenter->Initialize(this, BattleManager->GetBattleCameraManager(),SkillInfoActorClass))
		return false;
	
	ActionGroupController = NewObject<UBattleActionGroupController>(this);
	if (!IsValid(ActionGroupController))
		return false;
	if (!ActionGroupController->Initialize(this, BattleManager->GetBattleCameraManager(),SkillInfoActorClass))
		return false;

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
	
	// 정렬된 큐에서 첫 Exchange의 Action들을 저장
	BuildCurrentActionGroup();
	
	bBattleActionSequenceRunning = true;
	bBattleActionCompletionPending = false;
	bStopAfterCurrentExecution = false;
	
	if (IsValid(ActionGroupController))
	{
		// PlayGroupStart가 즉시 완료를 통지할 수 있으므로
		// 호출하기 전에 대기 상태를 설정
		bWaitingForActionGroupStart = true;
		ActionGroupController->PlayGroupStart(CurrentGroupActions);
	}
	else
	{
		StartCurrentBattleAction();
	}
	return true;
}

void ABattleSequenceManager::SortBattleActionQueue()
{
	BattleActionQueue.Sort([](const FBattleAction& A, const FBattleAction& B)
	{
		if (A.ExchangeIndex != B.ExchangeIndex)
			return A.ExchangeIndex < B.ExchangeIndex;

		return A.IsHigherPriorityThan(B);
	});
}

void ABattleSequenceManager::StartCurrentBattleAction()
{
	//기존 변초 기다리는 bWaitingForDeceiveCardReveal 말고 카메라 연출 기다리는 bWaitingForBattleActionPresentation
	if (!bBattleActionSequenceRunning || 
		bWaitingForActionGroupStart || 
		bWaitingForBattleActionPresentation)
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
	
	StartCurrentBattleActionPresentation();
}

bool ABattleSequenceManager::ShouldRequestDeceiveCardReveal(const FBattleAction& Action) const
{
	return true;
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
	
	StartCurrentBattleActionPresentation();
}

bool ABattleSequenceManager::GetCurrentBattleActionPair(FBattleAction*& OutCurrentAction, FBattleAction*& OutOpponentAction)
{
	OutCurrentAction = nullptr;
	OutOpponentAction = nullptr;

	if (!bBattleActionSequenceRunning || !BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
		return false;

	OutCurrentAction = &BattleActionQueue[CurrentBattleActionIndex];
	OutOpponentAction = FindOpponentBattleAction(*OutCurrentAction);

	return OutOpponentAction != nullptr;
}

bool ABattleSequenceManager::RefreshBattleActionTargetingResult(FBattleAction& Action) const
{
	Action.TargetingResult.Reset();

	if (!ActionExecutor)
		return false;

	return ActionExecutor->ResolveActionTargetingResult(Action, Action.TargetingResult);
}

void ABattleSequenceManager::ApplyBattleActionRuntimeModifier(FBattleAction& Action) const
{
	if (Action.bRuntimeModifierApplied || !IsValid(Action.Card.Get()))
		return;

	UMuksiBattleCardDataAsset* ExecutionCard = Action.Card.Get();

	Action.bRuntimeModifierApplied = true;

	if (!ExecutionCard->RuntimeModifierClass)
		return;

	const UBattleActionRuntimeModifier* RuntimeModifier = ExecutionCard->RuntimeModifierClass->GetDefaultObject<UBattleActionRuntimeModifier>();

	if (!IsValid(RuntimeModifier))
		return;

	RuntimeModifier->ModifyBattleAction(Action);
}

void ABattleSequenceManager::ExecuteCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || !BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
		return;

	FBattleAction& Action = BattleActionQueue[CurrentBattleActionIndex];
	FBattleAction* OpponentAction = FindOpponentBattleAction(Action);
	RefreshBattleActionTargetingResult(Action);
	ApplyBattleActionRuntimeModifier(Action);

	if (OpponentAction)
	{
		RefreshBattleActionTargetingResult(*OpponentAction);
		BattleActionStartedDelegate.Broadcast(Action, *OpponentAction);
	}

	if (!ActionExecutor)
	{
		FinishCurrentBattleAction();
		return;
	}

	if (!ActionExecutor->ExecuteBattleAction(Action))
		FinishCurrentBattleAction();
}

FBattleAction* ABattleSequenceManager::FindOpponentBattleAction(const FBattleAction& CurrentAction)
{
	for (FBattleAction& Action : BattleActionQueue)
	{
		if (&Action == &CurrentAction)
			continue;

		if (Action.ExchangeIndex == CurrentAction.ExchangeIndex && Action.bPlayerAction != CurrentAction.bPlayerAction)
			return &Action;
	}

	return nullptr;
}

void ABattleSequenceManager::HandleExecutionEntryStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FTargetingResult& TargetingResult)
{
	if (bBattleActionSequenceRunning)
		PresentBattleActionTargetingResult(Action, TargetingResult);
}

void ABattleSequenceManager::HandleBattleActionCompleted()
{
	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		if (bBattleActionSequenceRunning)
			FinishCurrentBattleAction();

		return;
	}

	const FBattleAction& CompletedAction = BattleActionQueue[CurrentBattleActionIndex];
	TArray<UMuksiStatusEffectComponent*> StatusEffectComponents;

	if (IsValid(CompletedAction.Attacker.Get()) && CompletedAction.Attacker->GetStatusEffectComponent())
		StatusEffectComponents.AddUnique(CompletedAction.Attacker->GetStatusEffectComponent());

	if (FBattleAction* OpponentAction = FindOpponentBattleAction(CompletedAction))
	{
		if (IsValid(OpponentAction->Attacker.Get()) && OpponentAction->Attacker->GetStatusEffectComponent())
			StatusEffectComponents.AddUnique(OpponentAction->Attacker->GetStatusEffectComponent());
	}

	PendingBattleActionAppliedFXWaitCount = StatusEffectComponents.Num();

	for (UMuksiStatusEffectComponent* StatusEffectComponent : StatusEffectComponents)
	{
		FSimpleDelegate CompletionDelegate;
		CompletionDelegate.BindUObject(this, &ABattleSequenceManager::HandleBattleActionAppliedFXWaitFinished);
		StatusEffectComponent->BeginAppliedFXWait(MoveTemp(CompletionDelegate));
	}

	BattleActionCompletedDelegate.Broadcast(CompletedAction);

	if (StatusEffectComponents.IsEmpty())
	{
		if (bBattleActionSequenceRunning)
			FinishCurrentBattleAction();

		return;
	}

	for (UMuksiStatusEffectComponent* StatusEffectComponent : StatusEffectComponents)
		StatusEffectComponent->EndAppliedFXWait();
}

void ABattleSequenceManager::HandleBattleActionAppliedFXWaitFinished()
{
	if (PendingBattleActionAppliedFXWaitCount <= 0)
		return;

	--PendingBattleActionAppliedFXWaitCount;

	if (PendingBattleActionAppliedFXWaitCount > 0 || !bBattleActionSequenceRunning)
		return;

	FinishCurrentBattleAction();
}

void ABattleSequenceManager::FinishCurrentBattleAction()
{
	if (!bBattleActionSequenceRunning || bBattleActionCompletionPending)
		return;

	bBattleActionCompletionPending = true;
	ClearBattleActionPresentation();

	const int32 CompletedExchangeIndex = BattleActionQueue.IsValidIndex(CurrentBattleActionIndex)
		? BattleActionQueue[CurrentBattleActionIndex].ExchangeIndex
		: INDEX_NONE;

	++CurrentBattleActionIndex;

	const bool bSequenceFinished = !BattleActionQueue.IsValidIndex(CurrentBattleActionIndex);
	const bool bExchangeFinished = bSequenceFinished || BattleActionQueue[CurrentBattleActionIndex].ExchangeIndex != CompletedExchangeIndex;
	

	//기존에 bSequenceFinished 검사하고 FinishBattleActionSequence()를 호출하는건 ActionGroupController 이후 NotifyActionGroupEndFinished()에서 호출
	if (bExchangeFinished)
	{
		// 이벤트 호출 전 대기 상태 설정
		bWaitingForActionGroupEnd = true;

		if (CompletedExchangeIndex != INDEX_NONE)
		{
			BattleExchangeCompletedDelegate.Broadcast(CompletedExchangeIndex);
		}

		// 이벤트 처리 중 시퀀스가 종료된 경우
		if (!bBattleActionSequenceRunning)
		{
			return;
		}

		if (IsValid(ActionGroupController))
		{
			ActionGroupController->PlayGroupEnd();
		}
		else
		{
			NotifyActionGroupEndFinished();
		}

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
	PendingBattleActionAppliedFXWaitCount = 0;
	bBattleActionCompletionPending = false;
	bWaitingForDeceiveCardReveal = false;
	bStopAfterCurrentExecution = false;
	bWaitingForActionGroupStart = false;
	bWaitingForBattleActionPresentation = false;
}

void ABattleSequenceManager::NotifyActionGroupStartFinished()
{
	if (!bBattleActionSequenceRunning || !bWaitingForActionGroupStart)
	{
		return;
	}

	bWaitingForActionGroupStart = false;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[ActionGroup] Start Finished. Exchange=%d"),
		CurrentGroupExchangeIndex
	);

	StartCurrentBattleAction();
}

void ABattleSequenceManager::NotifyActionGroupEndFinished()
{
	if (!bBattleActionSequenceRunning ||
		!bWaitingForActionGroupEnd)
	{
		return;
	}

	bWaitingForActionGroupEnd = false;

	UE_LOG(
		LogTemp, Warning,
		TEXT("[ActionGroup] End Finished. Exchange=%d"),
		CurrentGroupExchangeIndex
	);

	GetWorldTimerManager().ClearTimer(NextBattleActionTimerHandle);

	NextBattleActionTimerHandle =
		GetWorldTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (!bBattleActionSequenceRunning)
				{
					return;
				}

				bBattleActionCompletionPending = false;

				if (!BattleActionQueue.IsValidIndex(
						CurrentBattleActionIndex))
				{
					FinishBattleActionSequence();
					return;
				}

				// 이전 그룹 종료 후 다음 그룹 구성
				BuildCurrentActionGroup();

				if (IsValid(ActionGroupController))
				{
					bWaitingForActionGroupStart = true;
					ActionGroupController->PlayGroupStart(
						CurrentGroupActions);
				}
				else
				{
					StartCurrentBattleAction();
				}
			}));
}

void ABattleSequenceManager::BuildCurrentActionGroup()
{
	CurrentGroupActions.Reset();
	CurrentGroupExchangeIndex = INDEX_NONE;

	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		return;
	}

	CurrentGroupExchangeIndex = BattleActionQueue[CurrentBattleActionIndex].ExchangeIndex;

	for (int32 Index = CurrentBattleActionIndex; Index < BattleActionQueue.Num(); ++Index)
	{
		const FBattleAction& Action = BattleActionQueue[Index];

		if (Action.ExchangeIndex != CurrentGroupExchangeIndex)
		{
			break;
		}

		CurrentGroupActions.Add(Action);
	}
}

void ABattleSequenceManager::NotifyBattleActionPresentationFinished()
{
	if (!bWaitingForBattleActionPresentation)
	{
		return;
	}

	bWaitingForBattleActionPresentation = false;

	ExecuteCurrentBattleAction();
}

void ABattleSequenceManager::StartCurrentBattleActionPresentation()
{
	if (!BattleActionQueue.IsValidIndex(CurrentBattleActionIndex))
	{
		return;
	}

	const FBattleAction& Action = BattleActionQueue[CurrentBattleActionIndex];

	if (!IsValid(ActionPresenter))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("[BattleSequenceManager] ActionPresenter is invalid.")
		);

		ExecuteCurrentBattleAction();
		return;
	}

	bWaitingForBattleActionPresentation = true;

	if (!ActionPresenter->StartPresentation(Action))
	{
		bWaitingForBattleActionPresentation = false;

		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[BattleSequenceManager] "
				"Failed to start BattleAction Presentation."
			)
		);

		// Presentation 실패 때문에 전투 전체가 멈추지는 않도록
		ExecuteCurrentBattleAction();
	}
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
