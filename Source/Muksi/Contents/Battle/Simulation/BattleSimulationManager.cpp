#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"

ABattleSimulationManager::ABattleSimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SimulationCharacterClass = ABattleSimulationCharacter::StaticClass();
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

bool ABattleSimulationManager::StartSimulation(
	ABattleGridManager* InSourceGridManager,
	const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	ResetSimulationRuntime();
	SetSimulationState(EBattleSimulationState::Starting);

	if (!CreateSimulationCharacters(SourceCharacters)
		|| !CreateSimulationExecutionEnvironment(InSourceGridManager))
	{
		ResetSimulationRuntime();
		return false;
	}

	HideSourceCharacters();

	CurrentExchange.Reset(0);
	SetSimulationState(
		EBattleSimulationState::WaitingForPlayerTargeting);

	return true;
}

bool ABattleSimulationManager::SetPlayerAction(const FBattleAction& PlayerAction)
{
	if (!IsSimulationRunning() || CurrentExchange.bHasPlayerAction)
	{
		return false;
	}

	CurrentExchange.SetPlayerAction(PlayerAction);

	if (!CurrentExchange.bHasEnemyAction)
	{
		SetSimulationState(EBattleSimulationState::RevealingEnemyCard);
		return true;
	}

	return TryExecuteCurrentExchange();
}

bool ABattleSimulationManager::SetEnemyAction(const FBattleAction& EnemyAction)
{
	if (!IsSimulationRunning() || CurrentExchange.bHasEnemyAction)
	{
		return false;
	}

	CurrentExchange.SetEnemyAction(EnemyAction);

	if (!CurrentExchange.bHasPlayerAction)
	{
		SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
		return true;
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

void ABattleSimulationManager::ConvertToSourceTargetingResult(FTargetingResult& TargetingResult) const
{
	for (TObjectPtr<ABattleCharacterBase>& TargetCharacter : TargetingResult.TargetCharacters)
	{
		if (ABattleSimulationCharacter* SimulationCharacter = Cast<ABattleSimulationCharacter>(TargetCharacter))
		{
			if (ABattleCharacterBase* SourceCharacter = GetSourceCharacter(SimulationCharacter))
			{
				TargetCharacter = SourceCharacter;
			}
		}
	}

	for (FTargetingStepContext& StepContext : TargetingResult.StepContexts)
	{
		for (TObjectPtr<ABattleCharacterBase>& TargetCharacter : StepContext.TargetCharacters)
		{
			if (ABattleSimulationCharacter* SimulationCharacter = Cast<ABattleSimulationCharacter>(TargetCharacter))
			{
				if (ABattleCharacterBase* SourceCharacter = GetSourceCharacter(SimulationCharacter))
				{
					TargetCharacter = SourceCharacter;
				}
			}
		}
	}
}

ABattleCharacterBase* ABattleSimulationManager::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	return IsValid(SimulationCharacter) ? SimulationCharacter->GetSourceCharacter() : nullptr;
}

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

		SimulationCharacter->InitializeFromCharacter(SourceCharacter);
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
	return true;
}

bool ABattleSimulationManager::CreateSimulationPostProcess()
{
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

	SequenceActionQueue.Add(CurrentExchange.GetFirstAction());
	SequenceActionQueue.Add(CurrentExchange.GetSecondAction());
	SetSimulationState(EBattleSimulationState::ExecutingFirstAction);

	return ExecuteSimulationAction(CurrentExchange.GetFirstAction());
}

bool ABattleSimulationManager::ExecuteSimulationAction(const FBattleAction& SourceAction)
{
	if (!SimulationSequenceManager)
	{
		return false;
	}

	FBattleAction SimulationAction;

	if (!ConvertToSimulationAction(SourceAction, SimulationAction))
	{
		return false;
	}

	return SimulationSequenceManager->StartSequence(SimulationAction);
}

bool ABattleSimulationManager::ConvertToSimulationAction(const FBattleAction& SourceAction, FBattleAction& OutSimulationAction) const
{
	ABattleSimulationCharacter* SimulationAttacker = GetSimulationCharacter(SourceAction.Attacker);

	if (!SimulationAttacker || !SourceAction.Card)
	{
		return false;
	}

	OutSimulationAction = SourceAction;
	OutSimulationAction.Attacker = SimulationAttacker;
	ConvertTargetCharacters(OutSimulationAction.TargetingResult);

	return true;
}

void ABattleSimulationManager::ConvertTargetCharacters(FTargetingResult& TargetingResult) const
{
	for (TObjectPtr<ABattleCharacterBase>& TargetCharacter : TargetingResult.TargetCharacters)
	{
		if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(TargetCharacter))
		{
			TargetCharacter = SimulationCharacter;
		}
	}

	for (FTargetingStepContext& StepContext : TargetingResult.StepContexts)
	{
		for (TObjectPtr<ABattleCharacterBase>& TargetCharacter : StepContext.TargetCharacters)
		{
			if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(TargetCharacter))
			{
				TargetCharacter = SimulationCharacter;
			}
		}
	}
}

void ABattleSimulationManager::HandleSimulationSequenceFinished()
{
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