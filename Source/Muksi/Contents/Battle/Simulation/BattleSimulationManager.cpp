#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"

ABattleSimulationManager::ABattleSimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SimulationPostProcessVolumeClass = ABattleSimulationPostProcessVolume::StaticClass();
}

void ABattleSimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BattleManager)
		BattleManager->PhaseEntryRequestedDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandlePhaseEntryRequested);

	if (BattleManager)
		BattleManager->PhaseExecutionRequestedDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandlePhaseExecutionRequested);

	PhaseExecutionTask = nullptr;
	StopSimulation();
	if (PresentationController) PresentationController->Shutdown();
	PresentationController = nullptr;
	DestroySimulationWorldRuntimes();
	BattleManager = nullptr;
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationManager::InitializeBattleFlow(ABattleManager* InBattleManager)
{
	if (!IsValid(InBattleManager) || !IsValid(InBattleManager->GetBattleRuntimeContext()) || !IsValid(InBattleManager->GetBattleGridManager()))
		return false;

	BattleManager = InBattleManager;
	BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandlePhaseEntryRequested);
	BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandlePhaseExecutionRequested);
	return true;
}

EBattleSimulationState ABattleSimulationManager::GetSimulationState() const
{
	UBattleSimulationWorldRuntime* WorldRuntime = PresentationController ? PresentationController->GetPlayerPresentationWorldRuntime() : nullptr;
	return IsValid(WorldRuntime) ? WorldRuntime->GetSimulationState() : EBattleSimulationState::Idle;
}

bool ABattleSimulationManager::IsSimulationRunning() const
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (IsValid(WorldRuntime) && WorldRuntime->IsSimulationRunning()) return true;
	}
	return false;
}

UBattleSimulationWorldRuntime* ABattleSimulationManager::GetSimulationWorldRuntime(EBattleSimulationWorldType WorldType) const
{
	switch (WorldType)
	{
	case EBattleSimulationWorldType::PlayerActualEnemyDeceived:
		return ADWorldRuntime.Get();
	case EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived:
		return DDWorldRuntime.Get();
	case EBattleSimulationWorldType::PlayerDeceivedEnemyActual:
		return DAWorldRuntime.Get();
	default:
		return nullptr;
	}
}

ABattleGridManager* ABattleSimulationManager::GetBattleGridManager() const
{
	return IsValid(BattleManager.Get()) ? BattleManager->GetBattleGridManager() : nullptr;
}

bool ABattleSimulationManager::EnsureSimulationWorldRuntimes()
{
	if (!EnsureSimulationWorldRuntime(ADWorldRuntime, EBattleSimulationWorldType::PlayerActualEnemyDeceived))
		return false;
	if (!EnsureSimulationWorldRuntime(DDWorldRuntime, EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived))
		return false;
	if (!EnsureSimulationWorldRuntime(DAWorldRuntime, EBattleSimulationWorldType::PlayerDeceivedEnemyActual))
		return false;
	return true;
}

bool ABattleSimulationManager::EnsureSimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime, EBattleSimulationWorldType WorldType)
{
	if (IsValid(InOutWorldRuntime.Get()))
		return true;

	InOutWorldRuntime = NewObject<UBattleSimulationWorldRuntime>(this);
	if (!IsValid(InOutWorldRuntime.Get()))
		return false;
	InOutWorldRuntime->ExchangeFinishedDelegate.AddUObject(this, &ABattleSimulationManager::HandleSimulationWorldExchangeFinished);
	
	if (InOutWorldRuntime->Initialize(this, WorldType))
		return true;

	DestroySimulationWorldRuntime(InOutWorldRuntime);
	return false;
}

void ABattleSimulationManager::DestroySimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime)
{
	if (!IsValid(InOutWorldRuntime.Get()))
	{
		InOutWorldRuntime = nullptr;
		return;
	}
	InOutWorldRuntime->ExchangeFinishedDelegate.RemoveAll(this);
	InOutWorldRuntime->Shutdown();
	InOutWorldRuntime = nullptr;
}

void ABattleSimulationManager::DestroySimulationWorldRuntimes()
{
	DestroySimulationWorldRuntime(ADWorldRuntime);
	DestroySimulationWorldRuntime(DDWorldRuntime);
	DestroySimulationWorldRuntime(DAWorldRuntime);
}

TArray<UBattleSimulationWorldRuntime*> ABattleSimulationManager::GetSimulationWorldRuntimes() const
{
	return { ADWorldRuntime.Get(), DDWorldRuntime.Get(), DAWorldRuntime.Get() };
}

void ABattleSimulationManager::HideSimulationWorlds()
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (IsValid(WorldRuntime)) WorldRuntime->SetCharactersVisible(false);
	}
}

void ABattleSimulationManager::StopSimulationWorlds()
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (IsValid(WorldRuntime)) WorldRuntime->StopSimulation();
	}
}

bool ABattleSimulationManager::IsManagedSimulationRuntime(const UBattleSimulationWorldRuntime* WorldRuntime) const
{
	return WorldRuntime == ADWorldRuntime.Get() || WorldRuntime == DDWorldRuntime.Get() || WorldRuntime == DAWorldRuntime.Get();
}

void ABattleSimulationManager::PresentSimulationExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting)
{
	if (IsManagedSimulationRuntime(WorldRuntime) && PresentationController) PresentationController->PresentSimulationExecution(WorldRuntime, Action, ResolvedTargeting);
}

void ABattleSimulationManager::ClearSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime)
{
	if (IsManagedSimulationRuntime(WorldRuntime) && PresentationController) PresentationController->ClearSimulationExecutionPresentation(WorldRuntime);
}

void ABattleSimulationManager::RefreshFastForwardForPrimarySimulationWorld()
{
	// AD를 고정 Primary 완료 기준으로 사용해 Player의 AD/DD 토글이 배속 의미를 바꾸지 않게 한다.
	if (ExchangeCompletionBarrierIndex == INDEX_NONE || IsExchangeCompletionBarrierSatisfied()) return;
	if (!FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerActualEnemyDeceived)) return;
	if (PresentationController) PresentationController->StartSimulationFastForward();
}

void ABattleSimulationManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	if (!ShouldHandlePhaseEntry(NewPhase) || !TaskContext) return;
	UBattlePhaseTask* Task = TaskContext->RegisterTask(this);
	if (!Task) return;
	if (!IsValid(BattleManager.Get()) || !IsValid(BattleManager->GetBattleRuntimeContext()) || !IsValid(BattleManager->GetBattleGridManager()))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to resolve phase entry dependencies."));
		Task->Complete();
		return;
	}

	UpdateSimulationViewForPhaseTransition(OldPhase, NewPhase);
	ExecutePhaseEntryOperation(NewPhase);
	Task->Complete();
}

void ABattleSimulationManager::UpdateSimulationViewForPhaseTransition(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (NewPhase == EBattlePhase::Targeting)
	{
		if (PresentationController) PresentationController->SetPlayerSimulationViewInternal(EBattlePlayerSimulationView::ActualSelf);
		if (PresentationController) PresentationController->SetPlayerSimulationViewChangeLocked(true);
		return;
	}

	if (OldPhase == EBattlePhase::Targeting && PresentationController) PresentationController->SetPlayerSimulationViewChangeLocked(false);
}

void ABattleSimulationManager::ExecutePhaseEntryOperation(EBattlePhase NewPhase)
{
	switch (NewPhase)
	{
	case EBattlePhase::CardReveal:
		PrepareExchangeOrRestartCardSelection();
		break;
	case EBattlePhase::ExchangeEnd:
	case EBattlePhase::BattleActionSequenceStart:
		if (PresentationController) PresentationController->ClearRuntimeSimulationPreview();
		break;
	case EBattlePhase::RoundEnd:
		DeactivateRoundSimulation();
		break;
	case EBattlePhase::BattleEnd:
		StopSimulation();
		break;
	default:
		break;
	}
}

void ABattleSimulationManager::PrepareExchangeOrRestartCardSelection()
{
	if (PrepareCurrentExchangeSimulation()) return;

	UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to prepare exchange simulation."));
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::CardReveal) BattleManager->RestartCurrentExchangeCardSelection();
	}));
}

bool ABattleSimulationManager::ShouldHandlePhaseEntry(EBattlePhase Phase) const
{
	switch (Phase)
	{
	case EBattlePhase::ExchangeStart:
	case EBattlePhase::Targeting:
	case EBattlePhase::CardReveal:
	case EBattlePhase::ExchangeEnd:
	case EBattlePhase::BattleActionSequenceStart:
	case EBattlePhase::RoundEnd:
	case EBattlePhase::BattleEnd:
		return true;
	default:
		return false;
	}
}

void ABattleSimulationManager::HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	(void)OldPhase;
	if ((NewPhase != EBattlePhase::BattleStart && NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::SimulationSequence) || !TaskContext) return;
	PhaseExecutionTask = TaskContext->RegisterTask(this);
	if (!PhaseExecutionTask) return;

	switch (NewPhase)
	{
	case EBattlePhase::BattleStart:
		ExecuteBattleStart();
		break;
	case EBattlePhase::RoundStart:
		ExecuteRoundStart();
		break;
	case EBattlePhase::SimulationSequence:
		ExecuteSimulationSequence();
		break;
	default:
		break;
	}
}

void ABattleSimulationManager::ExecuteBattleStart()
{
	if (!PrepareBattleSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to prepare battle simulation."));
	}

	CompletePhaseExecution(EBattlePhase::BattleStart);
}

void ABattleSimulationManager::ExecuteRoundStart()
{
	if (!InitializeRoundSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to initialize round simulation."));
	}

	CompletePhaseExecution(EBattlePhase::RoundStart);
}

void ABattleSimulationManager::ExecuteSimulationSequence()
{
	if (StartCurrentExchangeSimulation()) return;

	UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to execute current exchange simulation."));
	CompletePhaseExecution(EBattlePhase::SimulationSequence);
}
void ABattleSimulationManager::HandleSimulationWorldExchangeFinished(UBattleSimulationWorldRuntime* WorldRuntime, int32 FinishedExchangeIndex)
{
	if (!IsManagedSimulationRuntime(WorldRuntime)) return;
	if (FinishedExchangeIndex != ExchangeCompletionBarrierIndex) return;

	FinishedWorldTypesForCurrentExchange.Add(WorldRuntime->GetWorldType());
	RefreshFastForwardForPrimarySimulationWorld();
	if (!IsExchangeCompletionBarrierSatisfied()) return;

	FinalizeCurrentExchangeSimulation(FinishedExchangeIndex);
}

void ABattleSimulationManager::FinalizeCurrentExchangeSimulation(int32 FinishedExchangeIndex)
{
	if (PresentationController) PresentationController->StopSimulationFastForward();
	if (!CommitActualExchangeActions(FinishedExchangeIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to commit AA actual exchange actions. Exchange=%d"), FinishedExchangeIndex);
	}

	if (AreAllSimulationWorldsCompleted() && PresentationController) PresentationController->ExitSimulationPresentation(false);

	NotifySimulationPhaseFinished(FinishedExchangeIndex);
	ClearExchangeCompletionBarrier();
}

bool ABattleSimulationManager::CommitActualExchangeActions(int32 ExchangeIndex)
{
	// Simulation 결과는 Commit하지 않고 AA 원본 Action만 최종 Queue에 보낸다.
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext)) return false;
	const FBattleAction* PlayerAction = RuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = RuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction) return false;
	if (!ValidateActualExchangeAction(*PlayerAction, ExchangeIndex, true)) return false;
	if (!ValidateActualExchangeAction(*EnemyAction, ExchangeIndex, false)) return false;
	RuntimeContext->AppendBattleActionSequenceAction(*PlayerAction);
	RuntimeContext->AppendBattleActionSequenceAction(*EnemyAction);
	return true;
}

bool ABattleSimulationManager::ValidateActualExchangeAction(const FBattleAction& Action, int32 ExchangeIndex, bool bExpectedPlayerAction) const
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || Action.ExchangeIndex != ExchangeIndex || Action.bPlayerAction != bExpectedPlayerAction) return false;
	if (!IsValid(Action.Attacker.Get()) || !IsValid(Action.Card.Get())) return false;
	ABattleCharacterBase* ExpectedAttacker = bExpectedPlayerAction ? static_cast<ABattleCharacterBase*>(RuntimeContext->GetPlayerCharacter()) : static_cast<ABattleCharacterBase*>(RuntimeContext->GetEnemyCharacter());
	if (Action.Attacker != ExpectedAttacker) return false;
	return true;
}

bool ABattleSimulationManager::PrepareBattleSimulation()
{
	if (!IsValid(BattleManager.Get()))
		return false;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	ABattleGridManager* SourceGridManager = BattleManager->GetBattleGridManager();
	if (!IsValid(RuntimeContext) || !IsValid(SourceGridManager)) 
		return false;

	if (!EnsureSimulationWorldRuntimes() || !EnsurePresentationController())
		return false;

	const TArray<ABattleCharacterBase*> SourceCharacters = { RuntimeContext->GetPlayerCharacter(), RuntimeContext->GetEnemyCharacter() };
	if (!PrepareSimulationWorldRuntimes(SourceGridManager, SourceCharacters)) 
		return false;

	HideSimulationWorlds();
	return true;
}

bool ABattleSimulationManager::PrepareSimulationWorldRuntimes(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->PrepareSimulationRuntime(SourceGridManager, SourceCharacters)) return false;
	}
	return true;
}

bool ABattleSimulationManager::AreSimulationWorldRuntimesReady() const
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->IsSimulationRuntimeReady()) return false;
	}
	return true;
}

bool ABattleSimulationManager::EnsurePresentationController()
{
	if (IsValid(PresentationController.Get())) return true;
	if (!IsValid(ADWorldRuntime.Get()) || !IsValid(DDWorldRuntime.Get()) || !IsValid(DAWorldRuntime.Get())) return false;

	PresentationController = NewObject<UBattleSimulationPresentationController>(this);
	if (!PresentationController || !PresentationController->Initialize(this))
	{
		PresentationController = nullptr;
		return false;
	}

	return true;
}

bool ABattleSimulationManager::InitializeRoundSimulation()
{
	if (!IsValid(BattleManager.Get())) return false;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	ABattleGridManager* SourceGridManager = BattleManager->GetBattleGridManager();
	if (!IsValid(RuntimeContext) || !IsValid(SourceGridManager)) return false;
	if (!EnsureSimulationWorldRuntimes() || !EnsurePresentationController()) return false;

	const TArray<ABattleCharacterBase*> SourceCharacters = { RuntimeContext->GetPlayerCharacter(), RuntimeContext->GetEnemyCharacter() };
	if (!AreSimulationWorldRuntimesReady() && !PrepareSimulationWorldRuntimes(SourceGridManager, SourceCharacters)) return false;
	ResetRoundSimulationState();

	if (!ResetSimulationWorldsFromActualBattleState(SourceGridManager, SourceCharacters)) return false;
	if (PresentationController && PresentationController->EnterSimulationPresentation(SourceCharacters))
	{
		return true;
	}

	StopSimulation();
	return false;
}

void ABattleSimulationManager::ResetRoundSimulationState()
{
	if (PresentationController) PresentationController->ExitSimulationPresentation(true);
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (IsValid(RuntimeContext)) RuntimeContext->ClearBattleActionSequenceQueue();
	ClearExchangeCompletionBarrier();
	if (PresentationController) PresentationController->SetPlayerSimulationViewInternal(EBattlePlayerSimulationView::ActualSelf);
	HideSimulationWorlds();
}

bool ABattleSimulationManager::ResetSimulationWorldsFromActualBattleState(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->ResetFromActualBattleState(SourceGridManager, SourceCharacters)) return false;
	}
	return true;
}

bool ABattleSimulationManager::PrepareCurrentExchangeSimulation()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsSimulationRunning() || !IsValid(RuntimeContext)) return false;
	const int32 ExchangeIndex = RuntimeContext->GetCurrentExchange();
	const FBattleAction* PlayerAction = RuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = RuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction) return false;
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->PrepareExchange(ExchangeIndex, *PlayerAction, *EnemyAction)) return false;
	}
	return true;
}

bool ABattleSimulationManager::StartCurrentExchangeSimulation()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence) return false;
	const int32 ExchangeIndex = RuntimeContext->GetCurrentExchange();
	ResetExchangeCompletionBarrier(ExchangeIndex);
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->ExecuteCurrentExchange()) return false;
	}
	return true;
}

void ABattleSimulationManager::NotifySimulationPhaseFinished(int32 FinishedExchangeIndex)
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager.Get()) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence || RuntimeContext->GetCurrentExchange() != FinishedExchangeIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSimulationManager] Ignored stale simulation completion. Finished=%d"), FinishedExchangeIndex);
		return;
	}
	CompletePhaseExecution(EBattlePhase::SimulationSequence);
}

void ABattleSimulationManager::CompletePhaseExecution(EBattlePhase FinishedPhase)
{
	if (BattleManager && BattleManager->GetCurrentPhase() != FinishedPhase)
	{
		PhaseExecutionTask = nullptr;
		return;
	}
	UBattlePhaseTask* CompletedTask = PhaseExecutionTask;
	PhaseExecutionTask = nullptr;
	if (CompletedTask)
		CompletedTask->Complete();
}

void ABattleSimulationManager::StopSimulation()
{
	DeactivateRoundSimulation();
	StopSimulationWorlds();
}

void ABattleSimulationManager::DeactivateRoundSimulation()
{
	if (PresentationController) PresentationController->ExitSimulationPresentation(true);
	HideSimulationWorlds();
	ClearExchangeCompletionBarrier();
}

void ABattleSimulationManager::ResetExchangeCompletionBarrier(int32 ExchangeIndex)
{
	if (PresentationController) PresentationController->StopSimulationFastForward();
	FinishedWorldTypesForCurrentExchange.Empty();
	ExchangeCompletionBarrierIndex = ExchangeIndex;
}

void ABattleSimulationManager::ClearExchangeCompletionBarrier()
{
	FinishedWorldTypesForCurrentExchange.Empty();
	ExchangeCompletionBarrierIndex = INDEX_NONE;
}

bool ABattleSimulationManager::IsExchangeCompletionBarrierSatisfied() const
{
	return FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerActualEnemyDeceived) && FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived) && FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerDeceivedEnemyActual);
}

bool ABattleSimulationManager::AreAllSimulationWorldsCompleted() const
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || WorldRuntime->GetSimulationState() != EBattleSimulationState::Completed) return false;
	}
	return true;
}
