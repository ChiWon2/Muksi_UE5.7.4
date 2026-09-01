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
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"
#include "Muksi/Contents/Battle/Targeting/BattleTargetingManager.h"

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

ABattleCharacterBase* ABattleSimulationManager::GetCharacterForWorld(const ABattleCharacterBase* Character, EBattleSimulationWorldType WorldType) const
{
	if (!IsValid(Character))
	{
		return nullptr;
	}

	const ABattleCharacterBase* SourceCharacter = Character;

	if (const ABattleSimulationCharacter* SimulationCharacter = Cast<ABattleSimulationCharacter>(Character))
	{
		SourceCharacter = SimulationCharacter->GetSourceCharacter();
	}

	if (!IsValid(SourceCharacter))
	{
		return nullptr;
	}

	if (!BattleSimulationWorld::UsesSimulationRuntime(WorldType))
	{
		return const_cast<ABattleCharacterBase*>(SourceCharacter);
	}

	UBattleSimulationWorldRuntime* WorldRuntime = GetSimulationWorldRuntime(WorldType);
	return IsValid(WorldRuntime)
		? WorldRuntime->GetSimulationCharacter(SourceCharacter)
		: nullptr;
}

ABattleGridManager* ABattleSimulationManager::GetBattleGridManager() const
{
	return IsValid(BattleManager.Get()) ? BattleManager->GetBattleGridManager() : nullptr;
}

bool ABattleSimulationManager::CreateSimulationWorldRuntimes(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	DestroySimulationWorldRuntimes();
	if (!CreateSimulationWorldRuntime(ADWorldRuntime, EBattleSimulationWorldType::PlayerActualEnemyDeceived, SourceGridManager, SourceCharacters))
	{
		DestroySimulationWorldRuntimes();
		return false;
	}
	if (!CreateSimulationWorldRuntime(DDWorldRuntime, EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived, SourceGridManager, SourceCharacters))
	{
		DestroySimulationWorldRuntimes();
		return false;
	}
	if (!CreateSimulationWorldRuntime(DAWorldRuntime, EBattleSimulationWorldType::PlayerDeceivedEnemyActual, SourceGridManager, SourceCharacters))
	{
		DestroySimulationWorldRuntimes();
		return false;
	}
	return true;
}

bool ABattleSimulationManager::CreateSimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime, EBattleSimulationWorldType WorldType, ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	InOutWorldRuntime = NewObject<UBattleSimulationWorldRuntime>(this);
	if (!IsValid(InOutWorldRuntime.Get()) || !InOutWorldRuntime->Initialize(this, WorldType, SourceGridManager, SourceCharacters))
	{
		InOutWorldRuntime = nullptr;
		return false;
	}
	InOutWorldRuntime->ExchangeFinishedDelegate.AddUObject(this, &ABattleSimulationManager::HandleSimulationWorldExchangeFinished);
	return true;
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

bool ABattleSimulationManager::IsManagedSimulationRuntime(const UBattleSimulationWorldRuntime* WorldRuntime) const
{
	return WorldRuntime == ADWorldRuntime.Get() || WorldRuntime == DDWorldRuntime.Get() || WorldRuntime == DAWorldRuntime.Get();
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
	(void)TaskContext;

	if (OldPhase == EBattlePhase::Targeting && NewPhase != EBattlePhase::Targeting && PresentationController)
	{
		PresentationController->SetPlayerSimulationViewChangeLocked(false);
	}

	switch (NewPhase)
	{
	case EBattlePhase::Targeting:
		if (PresentationController)
		{
			PresentationController->SetPlayerSimulationView(EBattlePlayerSimulationView::ActualSelf);
			PresentationController->SetPlayerSimulationViewChangeLocked(true);
		}
		break;

	case EBattlePhase::ExchangeEnd:
	case EBattlePhase::BattleActionSequenceStart:
		if (PresentationController) 
			PresentationController->ClearAllExecutionResults();
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
	if (!InitializeBattleSimulation())
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
	if (!PrepareCurrentExchangeSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to prepare current exchange simulation."));
		PhaseExecutionTask = nullptr;
		GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
		{
			if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::SimulationSequence)
			{
				BattleManager->RestartCurrentExchangeCardSelection();
			}
		}));
		return;
	}

	if (StartCurrentExchangeSimulation()) return;

	UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to execute current exchange simulation."));
	CompletePhaseExecution(EBattlePhase::SimulationSequence);
}

void ABattleSimulationManager::PresentSimulationWorldExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult)
{
	if (!IsManagedSimulationRuntime(WorldRuntime) || !PresentationController) return;
	PresentationController->UpdateExecutionResult(WorldRuntime, Action, TargetingResult);
}

void ABattleSimulationManager::HandleSimulationWorldExchangeFinished(UBattleSimulationWorldRuntime* WorldRuntime, int32 FinishedExchangeIndex)
{
	if (!IsManagedSimulationRuntime(WorldRuntime)) return;
	if (FinishedExchangeIndex != ExchangeCompletionBarrierIndex) return;
	if (PresentationController) PresentationController->RemoveExecutionResult(WorldRuntime);

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

bool ABattleSimulationManager::InitializeBattleSimulation()
{
	if (!IsValid(BattleManager.Get()))
		return false;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	ABattleGridManager* SourceGridManager = BattleManager->GetBattleGridManager();
	if (!IsValid(RuntimeContext) || !IsValid(SourceGridManager)) 
		return false;

	const TArray<ABattleCharacterBase*> SourceCharacters = { RuntimeContext->GetPlayerCharacter(), RuntimeContext->GetEnemyCharacter() };
	if (!CreateSimulationWorldRuntimes(SourceGridManager, SourceCharacters) || !CreatePresentationController())
	{
		DestroySimulationWorldRuntimes();
		return false;
	}
	return true;
}

bool ABattleSimulationManager::CreatePresentationController()
{
	if (IsValid(PresentationController.Get())) return true;

	if (!IsValid(ADWorldRuntime.Get()) || !IsValid(DDWorldRuntime.Get()) || !IsValid(DAWorldRuntime.Get())) return false;

	UTargetingPresentationController* TargetingPresentationController = BattleManager->GetBattleTargetingManager() ? BattleManager->GetBattleTargetingManager()->GetPresentationController() : nullptr;
	PresentationController = NewObject<UBattleSimulationPresentationController>(this);
	if (!PresentationController || !PresentationController->Initialize(this, TargetingPresentationController))
	{
		PresentationController = nullptr;
		return false;
	}

	return true;
}

bool ABattleSimulationManager::InitializeRoundSimulation()
{
	if (!IsValid(BattleManager.Get()))
		return false;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	
	if (!IsValid(RuntimeContext) || !IsValid(PresentationController.Get())) 
		return false;

	const TArray<ABattleCharacterBase*> SourceCharacters = { RuntimeContext->GetPlayerCharacter(), RuntimeContext->GetEnemyCharacter() };

	if (!ResetSimulationWorldsFromActualBattleState(SourceCharacters)) 
		return false;

	if (PresentationController && PresentationController->EnterSimulationPresentation(SourceCharacters))
	{
		return true;
	}

	StopSimulation();
	return false;
}

bool ABattleSimulationManager::ResetSimulationWorldsFromActualBattleState(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	for (UBattleSimulationWorldRuntime* WorldRuntime : GetSimulationWorldRuntimes())
	{
		if (!IsValid(WorldRuntime) || !WorldRuntime->ResetFromActualBattleState(SourceCharacters)) return false;
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
	DestroySimulationWorldRuntimes();
}

void ABattleSimulationManager::DeactivateRoundSimulation()
{
	if (PresentationController) PresentationController->ExitSimulationPresentation(true);
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
