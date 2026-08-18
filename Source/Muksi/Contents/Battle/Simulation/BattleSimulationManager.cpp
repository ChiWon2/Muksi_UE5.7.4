#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldManager.h"

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
	DestroySimulationWorldManagers();
	BattleGridManager = nullptr;
	BattleManager = nullptr;
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationManager::InitializeBattleFlow(ABattleManager* InBattleManager, ABattleGridManager* InBattleGridManager)
{
	if (!IsValid(InBattleManager) || !IsValid(InBattleManager->GetBattleRuntimeContext()) || !IsValid(InBattleGridManager))
		return false;

	BattleManager = InBattleManager;
	BattleGridManager = InBattleGridManager;
	MaxExchangeCount = BattleManager->GetMaxExchangeCount();
	BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandlePhaseEntryRequested);
	BattleManager->PhaseExecutionRequestedDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandlePhaseExecutionRequested);
	return true;
}

bool ABattleSimulationManager::IsSimulationRunning() const
{
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (IsValid(WorldManager) && WorldManager->IsSimulationRunning()) return true;
	}
	return false;
}

bool ABattleSimulationManager::CanChangePlayerSimulationView() const
{
	return bPlayerSimulationViewAvailable && !bPlayerSimulationViewChangeLocked;
}

bool ABattleSimulationManager::SetPlayerSimulationView(EBattlePlayerSimulationView NewView)
{
	if (!CanChangePlayerSimulationView()) return false;
	SetPlayerSimulationViewInternal(NewView);
	return true;
}

bool ABattleSimulationManager::TogglePlayerSimulationView()
{
	const EBattlePlayerSimulationView NewView = PlayerSimulationView == EBattlePlayerSimulationView::ActualSelf ? EBattlePlayerSimulationView::DeceivedSelf : EBattlePlayerSimulationView::ActualSelf;
	return SetPlayerSimulationView(NewView);
}

ABattleCharacterBase* ABattleSimulationManager::GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter)) return nullptr;
	if (!bPlayerSimulationViewAvailable) return const_cast<ABattleCharacterBase*>(SourceCharacter);
	ABattleSimulationWorldManager* WorldManager = GetPlayerPresentationWorldManager();
	ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldManager) ? WorldManager->GetSimulationCharacter(SourceCharacter) : nullptr;
	return IsValid(SimulationCharacter) ? static_cast<ABattleCharacterBase*>(SimulationCharacter) : const_cast<ABattleCharacterBase*>(SourceCharacter);
}

ABattleSimulationCharacter* ABattleSimulationManager::GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	ABattleSimulationWorldManager* WorldManager = GetPlayerPresentationWorldManager();
	return IsValid(WorldManager) ? WorldManager->GetSimulationCharacter(SourceCharacter) : nullptr;
}

ABattleGridManager* ABattleSimulationManager::GetSimulationGridManager() const
{
	ABattleSimulationWorldManager* WorldManager = GetPlayerPresentationWorldManager();
	return IsValid(WorldManager) ? WorldManager->GetSimulationGridManager() : nullptr;
}

ABattleSimulationCharacter* ABattleSimulationManager::GetPlayerTargetingSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	ABattleSimulationWorldManager* WorldManager = GetPlayerTargetingWorldManager();
	return IsValid(WorldManager) ? WorldManager->GetSimulationCharacter(SourceCharacter) : nullptr;
}

ABattleGridManager* ABattleSimulationManager::GetPlayerTargetingSimulationGridManager() const
{
	ABattleSimulationWorldManager* WorldManager = GetPlayerTargetingWorldManager();
	return IsValid(WorldManager) ? WorldManager->GetSimulationGridManager() : nullptr;
}

ABattleCharacterBase* ABattleSimulationManager::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	if (!IsValid(SimulationCharacter)) return nullptr;
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (IsValid(WorldManager) && WorldManager->GetSimulationCharacter(SimulationCharacter->GetSourceCharacter()) == SimulationCharacter) return SimulationCharacter->GetSourceCharacter();
	}
	return nullptr;
}

bool ABattleSimulationManager::EnsureSimulationWorldManagers()
{
	if (!EnsureSimulationWorldManager(ADWorldManager)) return false;
	if (!EnsureSimulationWorldManager(DDWorldManager)) return false;
	if (!EnsureSimulationWorldManager(DAWorldManager)) return false;
	return true;
}

bool ABattleSimulationManager::EnsureSimulationWorldManager(TObjectPtr<ABattleSimulationWorldManager>& InOutWorldManager)
{
	if (IsValid(InOutWorldManager)) 
		return true;
	UWorld* World = GetWorld();
	if (!World) 
		return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	InOutWorldManager = World->SpawnActor<ABattleSimulationWorldManager>(ABattleSimulationWorldManager::StaticClass(), GetActorTransform(), SpawnParameters);
	if (!InOutWorldManager) return false;
	InOutWorldManager->StateChangedDelegate.AddUObject(this, &ABattleSimulationManager::HandleSimulationWorldStateChanged);
	InOutWorldManager->ExchangeFinishedDelegate.AddUObject(this, &ABattleSimulationManager::HandleSimulationWorldExchangeFinished);
	return true;
}

void ABattleSimulationManager::DestroySimulationWorldManager(TObjectPtr<ABattleSimulationWorldManager>& InOutWorldManager)
{
	if (!IsValid(InOutWorldManager))
	{
		InOutWorldManager = nullptr;
		return;
	}
	InOutWorldManager->StateChangedDelegate.RemoveAll(this);
	InOutWorldManager->ExchangeFinishedDelegate.RemoveAll(this);
	InOutWorldManager->Destroy();
	InOutWorldManager = nullptr;
}

void ABattleSimulationManager::DestroySimulationWorldManagers()
{
	DestroySimulationWorldManager(ADWorldManager);
	DestroySimulationWorldManager(DDWorldManager);
	DestroySimulationWorldManager(DAWorldManager);
}

TArray<ABattleSimulationWorldManager*> ABattleSimulationManager::GetSimulationWorldManagers() const
{
	return { ADWorldManager.Get(), DDWorldManager.Get(), DAWorldManager.Get() };
}

void ABattleSimulationManager::HideSimulationWorlds()
{
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (IsValid(WorldManager)) WorldManager->SetWorldVisible(false);
	}
}

void ABattleSimulationManager::StopSimulationWorlds()
{
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (IsValid(WorldManager)) WorldManager->StopSimulation();
	}
}

ABattleSimulationWorldManager* ABattleSimulationManager::GetPlayerPresentationWorldManager() const
{
	return PlayerSimulationView == EBattlePlayerSimulationView::DeceivedSelf ? DDWorldManager.Get() : ADWorldManager.Get();
}

ABattleSimulationWorldManager* ABattleSimulationManager::GetPlayerTargetingWorldManager() const
{
	return ADWorldManager.Get();
}

bool ABattleSimulationManager::IsManagedSimulationWorld(const ABattleSimulationWorldManager* WorldManager) const
{
	return WorldManager == ADWorldManager.Get() || WorldManager == DDWorldManager.Get() || WorldManager == DAWorldManager.Get();
}

void ABattleSimulationManager::SetPlayerSimulationViewInternal(EBattlePlayerSimulationView NewView)
{
	if (PlayerSimulationView == NewView) return;
	PlayerSimulationView = NewView;
	ApplyPlayerSimulationView();
	SyncWorldSnapshot();
	BroadcastPresentationCharacters();
	PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
}

void ABattleSimulationManager::ApplyPlayerSimulationView()
{
	ABattleSimulationWorldManager* VisibleWorldManager = bPlayerSimulationViewAvailable ? GetPlayerPresentationWorldManager() : nullptr;
	if (IsValid(ADWorldManager) && ADWorldManager.Get() != VisibleWorldManager) ADWorldManager->SetWorldVisible(false);
	if (IsValid(DDWorldManager) && DDWorldManager.Get() != VisibleWorldManager) DDWorldManager->SetWorldVisible(false);
	if (IsValid(DAWorldManager)) DAWorldManager->SetWorldVisible(false);
	if (IsValid(VisibleWorldManager)) VisibleWorldManager->SetWorldVisible(true);
}

void ABattleSimulationManager::SetPlayerSimulationViewAvailable(bool bAvailable)
{
	if (!bAvailable) bPlayerSimulationViewChangeLocked = false;
	if (bPlayerSimulationViewAvailable == bAvailable) return;
	bPlayerSimulationViewAvailable = bAvailable;
	ApplyPlayerSimulationView();
	BroadcastPresentationCharacters();
	PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

void ABattleSimulationManager::SetPlayerSimulationViewChangeLocked(bool bLocked)
{
	if (bPlayerSimulationViewChangeLocked == bLocked) return;
	bPlayerSimulationViewChangeLocked = bLocked;
	PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

void ABattleSimulationManager::BroadcastPresentationCharacters()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext)) return;
	ABattleCharacterBase* SourcePlayerCharacter = RuntimeContext->GetPlayerCharacter();
	ABattleCharacterBase* SourceEnemyCharacter = RuntimeContext->GetEnemyCharacter();
	ABattleCharacterBase* PresentationPlayerCharacter = GetPresentationCharacter(SourcePlayerCharacter);
	ABattleCharacterBase* PresentationEnemyCharacter = GetPresentationCharacter(SourceEnemyCharacter);
	if (!IsValid(PresentationPlayerCharacter) || !IsValid(PresentationEnemyCharacter)) return;
	PresentationCharactersChangedDelegate.Broadcast(PresentationPlayerCharacter, PresentationEnemyCharacter);
}

void ABattleSimulationManager::RefreshFastForwardForPrimarySimulationWorld()
{
	// AD를 고정 Primary 완료 기준으로 사용해 Player의 AD/DD 토글이 배속 의미를 바꾸지 않게 한다.
	if (ExchangeCompletionBarrierIndex == INDEX_NONE || IsExchangeCompletionBarrierSatisfied()) return;
	if (!FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerActualEnemyDeceived)) return;
	StartSimulationFastForward();
}

void ABattleSimulationManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext)
{
	if (!ShouldHandlePhaseEntry(NewPhase) || !TaskContext) return;
	UBattlePhaseTask* Task = TaskContext->RegisterTask(this);
	if (!Task) return;
	if (!IsValid(BattleManager) || !IsValid(BattleManager->GetBattleRuntimeContext()) || !IsValid(BattleGridManager))
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
		SetPlayerSimulationViewInternal(EBattlePlayerSimulationView::ActualSelf);
		SetPlayerSimulationViewChangeLocked(true);
		return;
	}

	if (OldPhase == EBattlePhase::Targeting) SetPlayerSimulationViewChangeLocked(false);
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
		ClearRuntimeSimulationPreview();
		break;
	case EBattlePhase::RoundEnd:
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
	if ((NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::SimulationSequence) || !TaskContext) return;
	PhaseExecutionTask = TaskContext->RegisterTask(this);
	if (!PhaseExecutionTask) return;

	switch (NewPhase)
	{
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

void ABattleSimulationManager::ExecuteRoundStart()
{
	if (!IsSimulationRunning() && !InitializeRoundSimulation())
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

void ABattleSimulationManager::HandleSimulationWorldStateChanged(ABattleSimulationWorldManager* WorldManager, EBattleSimulationState NewState)
{
	if (WorldManager != GetPlayerPresentationWorldManager()) return;
	SimulationState = NewState;
	CurrentExchange = WorldManager->GetCurrentExchange();
}

void ABattleSimulationManager::HandleSimulationWorldExchangeFinished(ABattleSimulationWorldManager* WorldManager, int32 FinishedExchangeIndex, bool bSimulationCompleted, const FBattleSimulationExchange& FinishedExchange)
{
	if (!IsManagedSimulationWorld(WorldManager)) return;
	if (FinishedExchangeIndex != ExchangeCompletionBarrierIndex) return;
	(void)bSimulationCompleted;
	(void)FinishedExchange;

	FinishedWorldTypesForCurrentExchange.Add(WorldManager->GetWorldPolicy().WorldType);
	if (WorldManager == GetPlayerPresentationWorldManager()) SyncWorldSnapshot();
	RefreshFastForwardForPrimarySimulationWorld();
	if (!IsExchangeCompletionBarrierSatisfied()) return;

	FinalizeCurrentExchangeSimulation(FinishedExchangeIndex);
}

void ABattleSimulationManager::FinalizeCurrentExchangeSimulation(int32 FinishedExchangeIndex)
{
	StopSimulationFastForward();
	if (!CommitActualExchangeActions(FinishedExchangeIndex))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to commit AA actual exchange actions. Exchange=%d"), FinishedExchangeIndex);
	}

	if (AreAllSimulationWorldsCompleted()) ExitSimulationPresentation(false);

	NotifySimulationPhaseFinished(FinishedExchangeIndex);
	ClearExchangeCompletionBarrier();
}

bool ABattleSimulationManager::CommitActualExchangeActions(int32 ExchangeIndex)
{
	// Simulation 결과는 Commit하지 않고 AA 원본 Action만 최종 Queue에 보낸다.
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
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
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || Action.ExchangeIndex != ExchangeIndex || Action.bPlayerAction != bExpectedPlayerAction) return false;
	if (!IsValid(Action.Attacker) || !IsValid(Action.Card)) return false;
	ABattleCharacterBase* ExpectedAttacker = bExpectedPlayerAction ? static_cast<ABattleCharacterBase*>(RuntimeContext->GetPlayerCharacter()) : static_cast<ABattleCharacterBase*>(RuntimeContext->GetEnemyCharacter());
	if (Action.Attacker != ExpectedAttacker) return false;
	return true;
}

bool ABattleSimulationManager::InitializeRoundSimulation()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || !IsValid(BattleGridManager)) 
		return false;
	if (!EnsureSimulationWorldManagers()) 
		return false;

	const TArray<ABattleCharacterBase*> SourceCharacters = { RuntimeContext->GetPlayerCharacter(), RuntimeContext->GetEnemyCharacter() };
	ResetRoundSimulationState();

	if (!InitializeSimulationWorldsFromAA(BattleGridManager, SourceCharacters)) return false;
	if (EnterSimulationPresentation(SourceCharacters)) return true;

	StopSimulation();
	return false;
}

void ABattleSimulationManager::ResetRoundSimulationState()
{
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (IsValid(RuntimeContext)) RuntimeContext->ClearBattleActionSequenceQueue();
	ClearExchangeCompletionBarrier();
	SetPlayerSimulationViewAvailable(false);
	PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	HideSimulationWorlds();
}

bool ABattleSimulationManager::InitializeSimulationWorldsFromAA(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!InitializeSimulationWorldFromAA(ADWorldManager.Get(), EBattleSimulationWorldType::PlayerActualEnemyDeceived, InSourceGridManager, SourceCharacters)) return false;
	if (!InitializeSimulationWorldFromAA(DDWorldManager.Get(), EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived, InSourceGridManager, SourceCharacters)) return false;
	if (!InitializeSimulationWorldFromAA(DAWorldManager.Get(), EBattleSimulationWorldType::PlayerDeceivedEnemyActual, InSourceGridManager, SourceCharacters)) return false;
	return true;
}

bool ABattleSimulationManager::InitializeSimulationWorldFromAA(ABattleSimulationWorldManager* WorldManager, EBattleSimulationWorldType WorldType, ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(WorldManager))
		return false;
	const FBattleSimulationWorldPolicy Policy = FBattleSimulationWorldPolicy::Make(WorldType);
	if (!WorldManager->InitializeWorld(Policy, BattleManager, MaxExchangeCount, SimulationCharacterClass, PlayerSimulationMaterial.Get(), EnemySimulationMaterial.Get())) 
		return false;
	return WorldManager->ResetFromActualBattleState(InSourceGridManager, SourceCharacters);
}

bool ABattleSimulationManager::EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!CreateSimulationPostProcess()) 
		return false;

	HideSourceCharacters(SourceCharacters);
	
	SetPlayerSimulationViewAvailable(true);

	PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
	
	CaptureSimulationTimeScaleBaseline();
	
	SetSimulationTimeScale(1.0f);
	
	SyncWorldSnapshot();
	
	return true;
}

void ABattleSimulationManager::ExitSimulationPresentation(bool bClearRuntimePreview)
{
	SetPlayerSimulationViewAvailable(false);
	if (bClearRuntimePreview) ClearRuntimeSimulationPreview();
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
}

bool ABattleSimulationManager::PrepareCurrentExchangeSimulation()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsSimulationRunning() || !IsValid(RuntimeContext)) return false;
	const int32 ExchangeIndex = RuntimeContext->GetCurrentExchange();
	const FBattleAction* PlayerAction = RuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = RuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction) return false;
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (!IsValid(WorldManager) || !WorldManager->PrepareExchange(ExchangeIndex, *PlayerAction, *EnemyAction)) return false;
	}
	SyncWorldSnapshot();
	return true;
}

bool ABattleSimulationManager::StartCurrentExchangeSimulation()
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
	if (!IsValid(RuntimeContext) || BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence) return false;
	const int32 ExchangeIndex = RuntimeContext->GetCurrentExchange();
	ResetExchangeCompletionBarrier(ExchangeIndex);
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (!IsValid(WorldManager) || !WorldManager->ExecuteCurrentExchange()) return false;
	}
	SyncWorldSnapshot();
	return true;
}

void ABattleSimulationManager::NotifySimulationPhaseFinished(int32 FinishedExchangeIndex)
{
	UBattleRuntimeContext* RuntimeContext = IsValid(BattleManager) ? BattleManager->GetBattleRuntimeContext() : nullptr;
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
	ExitSimulationPresentation(true);
	StopSimulationWorlds();
	ClearExchangeCompletionBarrier();
	SyncWorldSnapshot();
	if (!IsValid(ADWorldManager))
	{
		CurrentExchange.Reset(INDEX_NONE);
		SimulationState = EBattleSimulationState::Completed;
	}
}

bool ABattleSimulationManager::CreateSimulationPostProcess()
{
	if (!bEnableSimulationPostProcess)
	{
		DestroySimulationPostProcess();
		return true;
	}
	if (IsValid(SimulationPostProcessVolume))
	{
		SimulationPostProcessVolume->ActivateSimulationPostProcess();
		return true;
	}
	UWorld* World = GetWorld();
	if (!World || !SimulationPostProcessVolumeClass) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SimulationPostProcessVolume = World->SpawnActor<ABattleSimulationPostProcessVolume>(SimulationPostProcessVolumeClass, GetActorTransform(), SpawnParameters);
	if (!SimulationPostProcessVolume) return false;
	SimulationPostProcessVolume->ActivateSimulationPostProcess();
	return true;
}

void ABattleSimulationManager::DestroySimulationPostProcess()
{
	if (!IsValid(SimulationPostProcessVolume))
	{
		SimulationPostProcessVolume = nullptr;
		return;
	}
	SimulationPostProcessVolume->DeactivateSimulationPostProcess();
	SimulationPostProcessVolume->Destroy();
	SimulationPostProcessVolume = nullptr;
}

void ABattleSimulationManager::HideSourceCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter)) continue;
		if (!SourceCharacterHiddenStates.Contains(SourceCharacter)) SourceCharacterHiddenStates.Add(SourceCharacter, SourceCharacter->IsHidden());
		SourceCharacter->SetActorHiddenInGame(true);
	}
}

void ABattleSimulationManager::RestoreSourceCharacters()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
		if (!IsValid(SourceCharacter)) continue;
		SourceCharacter->SetActorHiddenInGame(Pair.Value);
	}
	SourceCharacterHiddenStates.Empty();
}

void ABattleSimulationManager::ClearRuntimeSimulationPreview()
{
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (IsValid(WorldManager)) WorldManager->ClearRuntimeSimulationPreview();
	}
}

void ABattleSimulationManager::SyncWorldSnapshot()
{
	ABattleSimulationWorldManager* WorldManager = GetPlayerPresentationWorldManager();
	if (!IsValid(WorldManager)) return;
	SimulationState = WorldManager->GetSimulationState();
	CurrentExchange = WorldManager->GetCurrentExchange();
}

void ABattleSimulationManager::CaptureSimulationTimeScaleBaseline()
{
	if (bHasCapturedGlobalTimeDilation) return;
	CapturedGlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(this);
	bHasCapturedGlobalTimeDilation = true;
	CurrentSimulationTimeScale = 1.0f;
}

void ABattleSimulationManager::SetSimulationTimeScale(float NewTimeScale)
{
	const float SafeTimeScale = FMath::Max(NewTimeScale, 0.01f);
	if (!bHasCapturedGlobalTimeDilation) CaptureSimulationTimeScaleBaseline();
	if (FMath::IsNearlyEqual(CurrentSimulationTimeScale, SafeTimeScale)) return;
	CurrentSimulationTimeScale = SafeTimeScale;
	UGameplayStatics::SetGlobalTimeDilation(this, CapturedGlobalTimeDilation * CurrentSimulationTimeScale);
	SimulationTimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}

void ABattleSimulationManager::StartSimulationFastForward()
{
	SetSimulationTimeScale(FastForwardSimulationTimeScale);
}

void ABattleSimulationManager::StopSimulationFastForward()
{
	SetSimulationTimeScale(1.0f);
}

void ABattleSimulationManager::RestoreSimulationTimeScale()
{
	if (!bHasCapturedGlobalTimeDilation) return;
	const bool bWasFastForwarding = !FMath::IsNearlyEqual(CurrentSimulationTimeScale, 1.0f);
	UGameplayStatics::SetGlobalTimeDilation(this, CapturedGlobalTimeDilation);
	CurrentSimulationTimeScale = 1.0f;
	bHasCapturedGlobalTimeDilation = false;
	CapturedGlobalTimeDilation = 1.0f;
	if (bWasFastForwarding) SimulationTimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}

void ABattleSimulationManager::ResetExchangeCompletionBarrier(int32 ExchangeIndex)
{
	StopSimulationFastForward();
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
	for (ABattleSimulationWorldManager* WorldManager : GetSimulationWorldManagers())
	{
		if (!IsValid(WorldManager) || WorldManager->GetSimulationState() != EBattleSimulationState::Completed) return false;
	}
	return true;
}
