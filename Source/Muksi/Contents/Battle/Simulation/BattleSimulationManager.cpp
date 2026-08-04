#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"

// ============================================================================
// 생명주기 / 상태 조회
// ============================================================================
ABattleSimulationManager::ABattleSimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SimulationPostProcessVolumeClass = ABattleSimulationPostProcessVolume::StaticClass();
}

void ABattleSimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	DestroySimulationRuntime();
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationManager::IsSimulationRunning() const
{
	return SimulationState != EBattleSimulationState::Idle && SimulationState != EBattleSimulationState::Completed;
}

// ============================================================================
// Simulation 시작 및 Exchange 입력
// BattleManager::PrepareCurrentExchangeSimulation -> StartSimulation -> SetPlayer/EnemyAction
// ============================================================================
bool ABattleSimulationManager::StartSimulation(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	ResetSimulationRuntime();
	SetSimulationState(EBattleSimulationState::Starting);

	if (!CreateSimulationCharacters(SourceCharacters) || !CreateSimulationExecutionEnvironment(InSourceGridManager) || !CreateSimulationPostProcess())
	{
		ResetSimulationRuntime();
		return false;
	}

	HideSourceCharacters();

	CurrentExchange.Reset(0);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);

	return true;
}

bool ABattleSimulationManager::SetPlayerAction(const FBattleAction& PlayerAction, UMuksiBattleCardDataAsset* SimulationCardOverride)
{
	if (!IsSimulationRunning() || CurrentExchange.bHasPlayerAction || !IsValid(PlayerAction.Attacker) || !IsValid(PlayerAction.Card) || (SimulationCardOverride && !IsValid(SimulationCardOverride)))
	{
		return false;
	}

	CurrentExchange.SetPlayerAction(PlayerAction, SimulationCardOverride);

	SetSimulationState(
		CurrentExchange.CanResolveActionOrder()
			? EBattleSimulationState::RevealingEnemyCard
			: EBattleSimulationState::WaitingForPlayerTargeting
	);

	return true;
}

bool ABattleSimulationManager::SetEnemyAction(const FBattleAction& EnemyAction, UMuksiBattleCardDataAsset* SimulationCardOverride)
{
	if (!IsSimulationRunning() || CurrentExchange.bHasEnemyAction || !IsValid(EnemyAction.Attacker) || !IsValid(EnemyAction.Card) || (SimulationCardOverride && !IsValid(SimulationCardOverride)))
	{
		return false;
	}

	CurrentExchange.SetEnemyAction(EnemyAction, SimulationCardOverride);

	SetSimulationState(
		CurrentExchange.CanResolveActionOrder()
			? EBattleSimulationState::RevealingEnemyCard
			: EBattleSimulationState::WaitingForPlayerTargeting
	);

	return true;
}

bool ABattleSimulationManager::ExecuteCurrentExchange()
{
	if (!IsSimulationRunning())
	{
		return false;
	}

	if (SimulationState != EBattleSimulationState::RevealingEnemyCard)
	{
		return false;
	}

	return TryExecuteCurrentExchange();
}

void ABattleSimulationManager::StopSimulation()
{
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	DestroySimulationRuntime();
	CurrentExchange.Reset(INDEX_NONE);
	SetSimulationState(EBattleSimulationState::Completed);
}

ABattleSimulationCharacter* ABattleSimulationManager::GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter))
	{
		return nullptr;
	}

	const TObjectPtr<ABattleSimulationCharacter>* FoundCharacter = SimulationCharacterMap.Find(SourceCharacter);
	return FoundCharacter ? FoundCharacter->Get() : nullptr;
}

ABattleCharacterBase* ABattleSimulationManager::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	return IsValid(SimulationCharacter) ? SimulationCharacter->GetSourceCharacter() : nullptr;
}

// ============================================================================
// Simulation Runtime 생성
// 복제 캐릭터 -> 전용 Grid/Sequence 환경 -> 선택적 PostProcess
// ============================================================================
bool ABattleSimulationManager::CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	UWorld* World = GetWorld();

	if (!World || !SimulationCharacterClass || SourceCharacters.IsEmpty())
	{
		return false;
	}

	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter))
		{
			return false;
		}

		ABattleSimulationCharacter* SimulationCharacter = World->SpawnActor<ABattleSimulationCharacter>(SimulationCharacterClass, SourceCharacter->GetActorTransform());

		if (!SimulationCharacter)
		{
			return false;
		}

		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>()
			? PlayerSimulationMaterial.Get()
			: EnemySimulationMaterial.Get();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
		SimulationCharacterMap.Add(SourceCharacter, SimulationCharacter);
	}

	return SimulationCharacterMap.Num() == SourceCharacters.Num();
}

bool ABattleSimulationManager::CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager)
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(InSourceGridManager)) return false;

	SourceGridManager = InSourceGridManager;
	SourceGridManager->BeginSimulationRuntime();

	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (!SourceGridManager->ReplaceSimulationActor(Pair.Key.Get(), Pair.Value.Get()))
		{
			SourceGridManager->EndSimulationRuntime();
			SourceGridManager = nullptr;
			return false;
		}
	}

	SimulationSequenceManager = World->SpawnActorDeferred<ABattleSequenceManager>(ABattleSequenceManager::StaticClass(), FTransform::Identity, this);
	if (!SimulationSequenceManager)
	{
		SourceGridManager->EndSimulationRuntime();
		SourceGridManager = nullptr;
		return false;
	}

	SimulationSequenceManager->SetWorldManagerRegistrationEnabled(false);
	UGameplayStatics::FinishSpawningActor(SimulationSequenceManager, FTransform::Identity);
	SimulationSequenceManager->BattleGridManager = SourceGridManager;
	SimulationSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleSimulationManager::HandleSimulationSequenceFinished);
	SimulationSequenceManager->OnExecutionEntryStarted.AddUObject(this, &ABattleSimulationManager::HandleSimulationExecutionStarted);
	return true;
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

	if (!World || !SimulationPostProcessVolumeClass)
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;

	SimulationPostProcessVolume = World->SpawnActor<ABattleSimulationPostProcessVolume>(
		SimulationPostProcessVolumeClass,
		GetActorTransform(),
		SpawnParameters
	);

	if (!SimulationPostProcessVolume)
	{
		return false;
	}

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

void ABattleSimulationManager::HideSourceCharacters()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();

		if (!IsValid(SourceCharacter))
		{
			continue;
		}

		if (!SourceCharacterHiddenStates.Contains(SourceCharacter))
		{
			SourceCharacterHiddenStates.Add(SourceCharacter, SourceCharacter->IsHidden());
		}

		SourceCharacter->SetActorHiddenInGame(true);
	}
}

void ABattleSimulationManager::RestoreSourceCharacters()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();

		if (!IsValid(SourceCharacter))
		{
			continue;
		}

		SourceCharacter->SetActorHiddenInGame(Pair.Value);
	}

	SourceCharacterHiddenStates.Empty();
}

// ============================================================================
// Exchange 실행 파이프라인
// ExecuteCurrentExchange -> TryExecuteCurrentExchange -> ExecuteSimulationAction
// -> BattleSequenceManager -> FinishCurrentExchange
// ============================================================================
bool ABattleSimulationManager::TryExecuteCurrentExchange()
{
	if (!CurrentExchange.CanResolveActionOrder())
	{
		return false;
	}

	SetSimulationState(EBattleSimulationState::ResolvingActionOrder);

	if (!CurrentExchange.ResolveActionOrder())
	{
		return false;
	}

	SequenceActionQueue.Add(CurrentExchange.GetFirstAction().SequenceAction);
	SequenceActionQueue.Add(CurrentExchange.GetSecondAction().SequenceAction);
	SetSimulationState(EBattleSimulationState::ExecutingFirstAction);

	return ExecuteSimulationAction(CurrentExchange.GetFirstAction());
}

bool ABattleSimulationManager::ExecuteSimulationAction(const FBattleSimulationActionPlan& ActionPlan)
{
	if (!SimulationSequenceManager)
	{
		return false;
	}

	FBattleSequenceRequest Request;

	if (!BuildSimulationSequenceRequest(ActionPlan, Request))
	{
		return false;
	}

	OnSimulationActionStarted.Broadcast(ActionPlan.SequenceAction);
	if (!SimulationSequenceManager->StartSequenceWithRequest(Request))
	{
		OnSimulationActionFinished.Broadcast();
		return false;
	}

	return true;
}

bool ABattleSimulationManager::BuildSimulationSequenceRequest(const FBattleSimulationActionPlan& ActionPlan, FBattleSequenceRequest& OutRequest) const
{
	ABattleSimulationCharacter* SimulationAttacker = GetSimulationCharacter(ActionPlan.SequenceAction.Attacker);
	UMuksiBattleCardDataAsset* SimulationCard = ActionPlan.GetSimulationCard();

	if (!IsValid(SimulationAttacker) || !IsValid(SimulationCard))
	{
		return false;
	}

	OutRequest.Action = ActionPlan.SequenceAction;
	OutRequest.Action.Attacker = SimulationAttacker;
	OutRequest.ExecutionMode = EBattleExecutionMode::Simulation;
	OutRequest.ExecutionCardOverride = SimulationCard;
	return true;
}

void ABattleSimulationManager::HandleSimulationExecutionStarted(
	const FBattleAction& Action,
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	const FResolvedTargeting& ResolvedTargeting)
{
	OnSimulationExecutionStarted.Broadcast(Action, Entry, EntryIndex, ResolvedTargeting);
}

void ABattleSimulationManager::HandleSimulationSequenceFinished()
{
	OnSimulationActionFinished.Broadcast();

	if (SimulationState == EBattleSimulationState::ExecutingFirstAction)
	{
		SetSimulationState(EBattleSimulationState::ExecutingSecondAction);

		if (!ExecuteSimulationAction(CurrentExchange.GetSecondAction()))
		{
			StopSimulation();
		}

		return;
	}

	if (SimulationState == EBattleSimulationState::ExecutingSecondAction)
	{
		FinishCurrentExchange();
	}
}

void ABattleSimulationManager::FinishCurrentExchange()
{
	SetSimulationState(EBattleSimulationState::FinishingExchange);

	const int32 FinishedExchangeIndex = CurrentExchange.ExchangeIndex;
	const int32 NextExchangeIndex = FinishedExchangeIndex + 1;

	if (NextExchangeIndex >= MaxExchangeCount)
	{
		SetSimulationState(EBattleSimulationState::FinishingSimulation);
		OnSimulationExchangeFinished.Broadcast(FinishedExchangeIndex);
		RestoreSourceCharacters();
		DestroySimulationPostProcess();
		DestroySimulationRuntime();
		CurrentExchange.Reset(INDEX_NONE);
		SetSimulationState(EBattleSimulationState::Completed);
		OnBattleSimulationFinished.Broadcast();
		return;
	}

	CurrentExchange.Reset(NextExchangeIndex);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
	OnSimulationExchangeFinished.Broadcast(FinishedExchangeIndex);
}

// ============================================================================
// Runtime 정리 / 상태 전환
// ============================================================================
void ABattleSimulationManager::DestroySimulationRuntime()
{
	if (SimulationSequenceManager)
	{
		SimulationSequenceManager->OnSequenceFinished.RemoveAll(this);
		SimulationSequenceManager->Destroy();
		SimulationSequenceManager = nullptr;
	}

	if (SourceGridManager)
	{
		SourceGridManager->EndSimulationRuntime();
		SourceGridManager = nullptr;
	}

	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value))
		{
			Pair.Value->Destroy();
		}
	}

	SimulationCharacterMap.Empty();
}

void ABattleSimulationManager::ResetSimulationRuntime()
{
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	DestroySimulationRuntime();
	CurrentExchange.Reset(INDEX_NONE);
	SequenceActionQueue.Empty();
	SetSimulationState(EBattleSimulationState::Idle);
}

void ABattleSimulationManager::SetSimulationState(EBattleSimulationState NewState)
{
	SimulationState = NewState;
}
