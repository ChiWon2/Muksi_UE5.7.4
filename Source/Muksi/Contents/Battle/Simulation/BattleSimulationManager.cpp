#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldManager.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

ABattleSimulationManager::ABattleSimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SimulationPostProcessVolumeClass = ABattleSimulationPostProcessVolume::StaticClass();
}

void ABattleSimulationManager::BeginPlay()
{
	Super::BeginPlay();
	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this)) ManagerSubsystem->RegisterManager<ABattleSimulationManager>(this);
	if (!TryBindBattleFlow()) BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ABattleSimulationManager::BindBattleFlowDeferred));
}

void ABattleSimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BattleFlowBindingTimerHandle);
	if (BattleManager)
	{
		BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
		BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	}
	StopSimulation();
	DestroySimulationWorldManagers();
	BattleGridManager = nullptr;
	BattleRuntimeContext = nullptr;
	BattleManager = nullptr;
	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this)) ManagerSubsystem->UnregisterManager<ABattleSimulationManager>(this);
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationManager::IsSimulationRunning() const
{
	if (IsValid(ADWorldManager) && ADWorldManager->IsSimulationRunning()) return true;
	if (IsValid(DDWorldManager) && DDWorldManager->IsSimulationRunning()) return true;
	if (IsValid(DAWorldManager) && DAWorldManager->IsSimulationRunning()) return true;
	return false;
}

bool ABattleSimulationManager::CanChangePlayerSimulationView() const
{
	return bPlayerSimulationViewAvailable;
}

bool ABattleSimulationManager::SetPlayerSimulationView(EBattlePlayerSimulationView NewView)
{
	if (!CanChangePlayerSimulationView()) return false;
	if (PlayerSimulationView == NewView) return true;
	PlayerSimulationView = NewView;
	ApplyPlayerSimulationView();
	SyncWorldSnapshot();
	PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
	return true;
}

bool ABattleSimulationManager::TogglePlayerSimulationView()
{
	const EBattlePlayerSimulationView NewView = PlayerSimulationView == EBattlePlayerSimulationView::ActualSelf ? EBattlePlayerSimulationView::DeceivedSelf : EBattlePlayerSimulationView::ActualSelf;
	return SetPlayerSimulationView(NewView);
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

ABattleCharacterBase* ABattleSimulationManager::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	if (!IsValid(SimulationCharacter)) return nullptr;
	if (IsValid(ADWorldManager) && ADWorldManager->GetSimulationCharacter(SimulationCharacter->GetSourceCharacter()) == SimulationCharacter) return SimulationCharacter->GetSourceCharacter();
	if (IsValid(DDWorldManager) && DDWorldManager->GetSimulationCharacter(SimulationCharacter->GetSourceCharacter()) == SimulationCharacter) return SimulationCharacter->GetSourceCharacter();
	if (IsValid(DAWorldManager) && DAWorldManager->GetSimulationCharacter(SimulationCharacter->GetSourceCharacter()) == SimulationCharacter) return SimulationCharacter->GetSourceCharacter();
	return nullptr;
}

bool ABattleSimulationManager::TryBindBattleFlow()
{
	UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
	if (!ManagerSubsystem) return false;
	ABattleManager* FoundBattleManager = ManagerSubsystem->GetManager<ABattleManager>();
	if (!IsValid(FoundBattleManager) || !IsValid(FoundBattleManager->GetBattleRuntimeContext())) return false;
	if (BattleManager && BattleManager != FoundBattleManager)
	{
		BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
		BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	}
	BattleManager = FoundBattleManager;
	BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
	BattleGridManager = ManagerSubsystem->GetManager<ABattleGridManager>();
	if (!IsValid(BattleGridManager)) return false;
	MaxExchangeCount = BattleManager->GetMaxExchangeCount();
	BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
	BattleManager->ChangePhaseDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
	BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	BattleManager->PhaseUIFinishedDelegate.AddUObject(this, &ABattleSimulationManager::HandleBattlePhaseUIFinished);
	return true;
}

void ABattleSimulationManager::BindBattleFlowDeferred()
{
	if (!TryBindBattleFlow()) UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to bind battle flow."));
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
	if (IsValid(InOutWorldManager)) return true;
	UWorld* World = GetWorld();
	if (!World) return false;
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

ABattleSimulationWorldManager* ABattleSimulationManager::ResolveSimulationWorldManager(EBattleSimulationWorldType WorldType) const
{
	switch (WorldType)
	{
	case EBattleSimulationWorldType::PlayerActualEnemyDeceived: return ADWorldManager.Get();
	case EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived: return DDWorldManager.Get();
	case EBattleSimulationWorldType::PlayerDeceivedEnemyActual: return DAWorldManager.Get();
	default: return nullptr;
	}
}

ABattleSimulationWorldManager* ABattleSimulationManager::GetPlayerPresentationWorldManager() const
{
	return PlayerSimulationView == EBattlePlayerSimulationView::DeceivedSelf ? DDWorldManager.Get() : ADWorldManager.Get();
}

bool ABattleSimulationManager::IsManagedSimulationWorld(const ABattleSimulationWorldManager* WorldManager) const
{
	return WorldManager == ADWorldManager.Get() || WorldManager == DDWorldManager.Get() || WorldManager == DAWorldManager.Get();
}

void ABattleSimulationManager::ApplyPlayerSimulationView()
{
	if (IsValid(ADWorldManager)) ADWorldManager->SetWorldVisible(bPlayerSimulationViewAvailable && PlayerSimulationView == EBattlePlayerSimulationView::ActualSelf);
	if (IsValid(DDWorldManager)) DDWorldManager->SetWorldVisible(bPlayerSimulationViewAvailable && PlayerSimulationView == EBattlePlayerSimulationView::DeceivedSelf);
	if (IsValid(DAWorldManager)) DAWorldManager->SetWorldVisible(false);
}

void ABattleSimulationManager::SetPlayerSimulationViewAvailable(bool bAvailable)
{
	if (bPlayerSimulationViewAvailable == bAvailable) return;
	bPlayerSimulationViewAvailable = bAvailable;
	ApplyPlayerSimulationView();
	PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

void ABattleSimulationManager::RefreshFastForwardForPrimarySimulationWorld()
{
	// AD를 고정 Primary 완료 기준으로 사용해 Player의 AD/DD 토글이 배속 의미를 바꾸지 않게 한다.
	if (ExchangeCompletionBarrierIndex == INDEX_NONE || IsExchangeCompletionBarrierSatisfied()) return;
	if (!FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerActualEnemyDeceived)) return;
	StartSimulationFastForward();
}

void ABattleSimulationManager::NotifySimulationWorldPhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (NewPhase != EBattlePhase::ExchangeStart && NewPhase != EBattlePhase::ExchangeEnd) return;
	if (IsValid(ADWorldManager)) ADWorldManager->NotifyBattlePhaseChanged(OldPhase, NewPhase);
	if (IsValid(DDWorldManager)) DDWorldManager->NotifyBattlePhaseChanged(OldPhase, NewPhase);
	if (IsValid(DAWorldManager)) DAWorldManager->NotifyBattlePhaseChanged(OldPhase, NewPhase);
}

void ABattleSimulationManager::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (bPlayerSimulationViewAvailable) PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
	NotifySimulationWorldPhaseChanged(OldPhase, NewPhase);
	switch (NewPhase)
	{
	case EBattlePhase::CardReveal:
		if (!PrepareCurrentExchangeSimulation())
		{
			UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to prepare exchange simulation."));
			GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::CardReveal) BattleManager->RestartCurrentExchangeCardSelection();
			}));
		}
		break;
	case EBattlePhase::SimulationSequence:
		if (!StartCurrentExchangeSimulation()) UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to execute current exchange simulation."));
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

void ABattleSimulationManager::HandleBattlePhaseUIFinished(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	(void)OldPhase;
	if (NewPhase != EBattlePhase::RoundStart || IsSimulationRunning()) return;
	if (!InitializeRoundSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to initialize round simulation."));
		return;
	}
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::RoundStart) BattleManager->NotifyPhaseExecutionFinished();
	}));
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
	StopSimulationFastForward();
	if (!CommitActualExchangeActions(FinishedExchangeIndex)) UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to commit AA actual exchange actions. Exchange=%d"), FinishedExchangeIndex);
	const bool bAllSimulationCompleted = AreAllSimulationWorldsCompleted();
	if (bAllSimulationCompleted) SetPlayerSimulationViewAvailable(false);
	if (bAllSimulationCompleted) RestoreSourceCharacters();
	if (bAllSimulationCompleted) DestroySimulationPostProcess();
	if (bAllSimulationCompleted) RestoreSimulationTimeScale();
	NotifySimulationPhaseFinished(FinishedExchangeIndex);
	FinishedWorldTypesForCurrentExchange.Empty();
	ExchangeCompletionBarrierIndex = INDEX_NONE;
}

bool ABattleSimulationManager::CommitActualExchangeActions(int32 ExchangeIndex)
{
	// Simulation 결과는 Commit하지 않고 BattleRuntimeContext의 AA 원본 Action만 최종 Queue에 보낸다.
	if (!BattleRuntimeContext) return false;
	const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction) return false;
	if (!ValidateActualExchangeAction(*PlayerAction, ExchangeIndex, true)) return false;
	if (!ValidateActualExchangeAction(*EnemyAction, ExchangeIndex, false)) return false;
	BattleRuntimeContext->AppendBattleActionSequenceAction(*PlayerAction);
	BattleRuntimeContext->AppendBattleActionSequenceAction(*EnemyAction);
	return true;
}

bool ABattleSimulationManager::ValidateActualExchangeAction(const FBattleAction& Action, int32 ExchangeIndex, bool bExpectedPlayerAction) const
{
	if (!BattleRuntimeContext || Action.ExchangeIndex != ExchangeIndex || Action.bPlayerAction != bExpectedPlayerAction) return false;
	if (!IsValid(Action.Attacker) || !IsValid(Action.Card)) return false;
	ABattleCharacterBase* ExpectedAttacker = bExpectedPlayerAction ? static_cast<ABattleCharacterBase*>(BattleRuntimeContext->GetPlayerCharacter()) : static_cast<ABattleCharacterBase*>(BattleRuntimeContext->GetEnemyCharacter());
	if (Action.Attacker != ExpectedAttacker) return false;
	return true;
}

bool ABattleSimulationManager::InitializeRoundSimulation()
{
	if (!IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager)) return false;
	TArray<ABattleCharacterBase*> SourceCharacters;
	SourceCharacters.Add(BattleRuntimeContext->GetPlayerCharacter());
	SourceCharacters.Add(BattleRuntimeContext->GetEnemyCharacter());
	return ResetSimulationWorldsFromActualBattle(BattleGridManager, SourceCharacters);
}

bool ABattleSimulationManager::PrepareCurrentExchangeSimulation()
{
	if (!IsSimulationRunning() || !IsValid(BattleRuntimeContext)) return false;
	const int32 ExchangeIndex = BattleRuntimeContext->GetCurrentExchange();
	const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction) return false;
	if (!IsValid(ADWorldManager) || !ADWorldManager->PrepareExchange(ExchangeIndex, *PlayerAction, *EnemyAction)) return false;
	if (!IsValid(DDWorldManager) || !DDWorldManager->PrepareExchange(ExchangeIndex, *PlayerAction, *EnemyAction)) return false;
	if (!IsValid(DAWorldManager) || !DAWorldManager->PrepareExchange(ExchangeIndex, *PlayerAction, *EnemyAction)) return false;
	SyncWorldSnapshot();
	return true;
}

bool ABattleSimulationManager::StartCurrentExchangeSimulation()
{
	if (!BattleManager || !BattleRuntimeContext || BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence) return false;
	const int32 ExchangeIndex = BattleRuntimeContext->GetCurrentExchange();
	ResetExchangeCompletionBarrier(ExchangeIndex);
	if (!IsValid(ADWorldManager) || !ADWorldManager->ExecuteCurrentExchange()) return false;
	if (!IsValid(DDWorldManager) || !DDWorldManager->ExecuteCurrentExchange()) return false;
	if (!IsValid(DAWorldManager) || !DAWorldManager->ExecuteCurrentExchange()) return false;
	SyncWorldSnapshot();
	return true;
}

void ABattleSimulationManager::NotifySimulationPhaseFinished(int32 FinishedExchangeIndex)
{
	if (!BattleManager || !BattleRuntimeContext || BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence || BattleRuntimeContext->GetCurrentExchange() != FinishedExchangeIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSimulationManager] Ignored stale simulation completion. Finished=%d"), FinishedExchangeIndex);
		return;
	}
	BattleManager->NotifyPhaseExecutionFinished();
}

bool ABattleSimulationManager::ResetSimulationWorldsFromActualBattle(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(BattleRuntimeContext) || !IsValid(BattleManager) || !IsValid(InSourceGridManager)) return false;
	if (!EnsureSimulationWorldManagers()) return false;
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
	BattleRuntimeContext->ClearBattleActionSequenceQueue();
	FinishedWorldTypesForCurrentExchange.Empty();
	ExchangeCompletionBarrierIndex = INDEX_NONE;
	SetPlayerSimulationViewAvailable(false);
	PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	ADWorldManager->SetWorldVisible(false);
	DDWorldManager->SetWorldVisible(false);
	DAWorldManager->SetWorldVisible(false);
	if (!InitializeSimulationWorldFromActualBattle(ADWorldManager.Get(), EBattleSimulationWorldType::PlayerActualEnemyDeceived, InSourceGridManager, SourceCharacters)) return false;
	if (!InitializeSimulationWorldFromActualBattle(DDWorldManager.Get(), EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived, InSourceGridManager, SourceCharacters)) return false;
	if (!InitializeSimulationWorldFromActualBattle(DAWorldManager.Get(), EBattleSimulationWorldType::PlayerDeceivedEnemyActual, InSourceGridManager, SourceCharacters)) return false;
	if (!CreateSimulationPostProcess())
	{
		StopSimulation();
		return false;
	}
	HideSourceCharacters(SourceCharacters);
	SetPlayerSimulationViewAvailable(true);
	PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
	CaptureSimulationTimeScaleBaseline();
	SetSimulationTimeScale(1.0f);
	SyncWorldSnapshot();
	return true;
}

bool ABattleSimulationManager::InitializeSimulationWorldFromActualBattle(ABattleSimulationWorldManager* WorldManager, EBattleSimulationWorldType WorldType, ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(WorldManager)) return false;
	const FBattleSimulationWorldPolicy Policy = FBattleSimulationWorldPolicy::Make(WorldType);
	if (!WorldManager->InitializeWorld(Policy, BattleManager, MaxExchangeCount, SimulationCharacterClass, PlayerSimulationMaterial.Get(), EnemySimulationMaterial.Get())) return false;
	return WorldManager->ResetFromActualBattleState(InSourceGridManager, SourceCharacters);
}

void ABattleSimulationManager::StopSimulation()
{
	SetPlayerSimulationViewAvailable(false);
	ClearRuntimeSimulationPreview();
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
	if (IsValid(ADWorldManager)) ADWorldManager->StopSimulation();
	if (IsValid(DDWorldManager)) DDWorldManager->StopSimulation();
	if (IsValid(DAWorldManager)) DAWorldManager->StopSimulation();
	FinishedWorldTypesForCurrentExchange.Empty();
	ExchangeCompletionBarrierIndex = INDEX_NONE;
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
	if (IsValid(ADWorldManager)) ADWorldManager->ClearRuntimeSimulationPreview();
	if (IsValid(DDWorldManager)) DDWorldManager->ClearRuntimeSimulationPreview();
	if (IsValid(DAWorldManager)) DAWorldManager->ClearRuntimeSimulationPreview();
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

bool ABattleSimulationManager::IsExchangeCompletionBarrierSatisfied() const
{
	return FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerActualEnemyDeceived) && FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived) && FinishedWorldTypesForCurrentExchange.Contains(EBattleSimulationWorldType::PlayerDeceivedEnemyActual);
}

bool ABattleSimulationManager::AreAllSimulationWorldsCompleted() const
{
	if (!IsValid(ADWorldManager) || ADWorldManager->GetSimulationState() != EBattleSimulationState::Completed) return false;
	if (!IsValid(DDWorldManager) || DDWorldManager->GetSimulationState() != EBattleSimulationState::Completed) return false;
	if (!IsValid(DAWorldManager) || DAWorldManager->GetSimulationState() != EBattleSimulationState::Completed) return false;
	return true;
}
