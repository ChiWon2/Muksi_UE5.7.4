#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"

#include "Materials/MaterialInterface.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"

bool UBattleSimulationWorldRuntime::Initialize(ABattleSimulationManager* InSimulationManager, EBattleSimulationWorldType InWorldType, ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(InSimulationManager) || !IsValid(SourceGridManager) || SourceCharacters.IsEmpty() || !BattleSimulationWorld::UsesSimulationRuntime(InWorldType))
		return false;
	SimulationManager = InSimulationManager;
	WorldType = InWorldType;
	if (!CreateSimulationCharacters(SourceCharacters) || !CreateActionExecutor(SourceGridManager))
	{
		Shutdown();
		return false;
	}
	ClearPreparedActions();
	SetCharactersVisible(false);
	SimulationState = EBattleSimulationState::Idle;
	return true;
}

void UBattleSimulationWorldRuntime::Shutdown()
{
	DestroySimulationRuntime();
	SimulationManager = nullptr;
	ClearPreparedActions();
	SimulationState = EBattleSimulationState::Idle;
}

void UBattleSimulationWorldRuntime::BeginDestroy()
{
	Shutdown();
	Super::BeginDestroy();
}

bool UBattleSimulationWorldRuntime::ResetFromActualBattleState(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsSimulationRuntimeInitialized() || ActionExecutor->IsRunning() || SourceCharacters.IsEmpty() || SimulationCharacterMap.Num() != SourceCharacters.Num())
	{
		SimulationState = EBattleSimulationState::Idle;
		return false;
	}
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(SourceCharacter);
		if (!IsValid(SourceCharacter) || !IsValid(SimulationCharacter))
		{
			SimulationState = EBattleSimulationState::Idle;
			return false;
		}
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? SimulationManager->GetPlayerSimulationMaterial() : SimulationManager->GetEnemySimulationMaterial();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
	}
	if (!ResetGridStateFromActual())
	{
		SimulationState = EBattleSimulationState::Idle;
		return false;
	}
	ABattleGridManager* GridManager = SimulationManager->GetBattleGridManager();
	GridManager->ClearAllTargetIndicators();
	ClearPreparedActions();
	SimulationState = EBattleSimulationState::Ready;
	return true;
}

void UBattleSimulationWorldRuntime::SetCharactersVisible(bool bVisible)
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value.Get()))
			Pair.Value->SetActorHiddenInGame(!bVisible);
	}
}

bool UBattleSimulationWorldRuntime::PrepareExchange(const FBattleAction& PlayerAction, const FBattleAction& EnemyAction)
{
	if (SimulationState != EBattleSimulationState::Ready)
		return false;

	FBattleAction NewPlayerAction;
	FBattleAction NewEnemyAction;
	if (!BuildSimulationAction(PlayerAction, NewPlayerAction))
		return false;
	if (!BuildSimulationAction(EnemyAction, NewEnemyAction))
		return false;

	PreparedPlayerAction = MoveTemp(NewPlayerAction);
	PreparedEnemyAction = MoveTemp(NewEnemyAction);
	SimulationState = EBattleSimulationState::Prepared;
	return true;
}

bool UBattleSimulationWorldRuntime::ExecuteCurrentExchange()
{
	if (SimulationState != EBattleSimulationState::Prepared)
		return false;

	const bool bPlayerFirst = PreparedPlayerAction.Speed >= PreparedEnemyAction.Speed;
	const FBattleAction& FirstAction = bPlayerFirst ? PreparedPlayerAction : PreparedEnemyAction;
	SimulationState = EBattleSimulationState::ExecutingFirstAction;

	if (ActionExecutor && ActionExecutor->ExecuteBattleAction(FirstAction))
		return true;

	SimulationState = EBattleSimulationState::Prepared;
	return false;
}

void UBattleSimulationWorldRuntime::FailCurrentExchange()
{
	const int32 FailedExchangeIndex = PreparedPlayerAction.ExchangeIndex;
	if (ActionExecutor)
		ActionExecutor->Stop();

	ClearPreparedActions();
	SimulationState = EBattleSimulationState::Ready;
	if (IsValid(SimulationManager.Get()))
		SimulationManager->NotifySimulationWorldExchangeCompleted(this, FailedExchangeIndex, false);
}

bool UBattleSimulationWorldRuntime::IsSimulationRunning() const
{
	return SimulationState != EBattleSimulationState::Idle && SimulationState != EBattleSimulationState::Completed;
}

bool UBattleSimulationWorldRuntime::IsSimulationRuntimeInitialized() const
{
	if (!IsValid(SimulationManager.Get()) || !IsValid(ActionExecutor.Get()) || SimulationCharacterMap.IsEmpty())
		return false;
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (!IsValid(Pair.Key.Get()) || !IsValid(Pair.Value.Get()))
			return false;
	}
	return true;
}

ABattleSimulationCharacter* UBattleSimulationWorldRuntime::GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter))
		return nullptr;
	const TObjectPtr<ABattleSimulationCharacter>* FoundCharacter = SimulationCharacterMap.Find(SourceCharacter);
	return FoundCharacter ? FoundCharacter->Get() : nullptr;
}

bool UBattleSimulationWorldRuntime::CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	UWorld* World = IsValid(SimulationManager.Get()) ? SimulationManager->GetWorld() : nullptr;
	TSubclassOf<ABattleSimulationCharacter> CharacterClass;

	if (IsValid(SimulationManager.Get()))
		CharacterClass = SimulationManager->GetSimulationCharacterClass();
	if (!World || !CharacterClass || SourceCharacters.IsEmpty())
		return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter)) 
			return false;
		ABattleSimulationCharacter* SimulationCharacter = World->SpawnActor<ABattleSimulationCharacter>(CharacterClass, SourceCharacter->GetActorTransform());

		if (!SimulationCharacter) 
			return false;
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? SimulationManager->GetPlayerSimulationMaterial() : SimulationManager->GetEnemySimulationMaterial();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
		SimulationCharacterMap.Add(SourceCharacter, SimulationCharacter);
	}
	return SimulationCharacterMap.Num() == SourceCharacters.Num();
}

bool UBattleSimulationWorldRuntime::CreateActionExecutor(ABattleGridManager* InSourceGridManager)
{
	if (!IsValid(InSourceGridManager))
		return false;
	ActionExecutor = NewObject<UBattleActionExecutor>(this);
	ABattleManager* OwningBattleManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleManager() : nullptr;
	if (!ActionExecutor || !ActionExecutor->Initialize(OwningBattleManager, InSourceGridManager, WorldType))
		return false;
	ActionExecutor->OnBattleActionCompleted.BindUObject(this, &UBattleSimulationWorldRuntime::HandleSimulationActionFinished);
	ActionExecutor->OnBattleExecutionStarted.BindUObject(this, &UBattleSimulationWorldRuntime::HandleSimulationExecutionStarted);
	return true;
}

bool UBattleSimulationWorldRuntime::ResetGridStateFromActual()
{
	ABattleGridManager* GridManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleGridManager() : nullptr;
	if (!IsValid(GridManager))
		return false;
	return GridManager->ResetSimulationWorldStateFromActual(WorldType, SimulationCharacterMap);
}

bool UBattleSimulationWorldRuntime::BuildSimulationAction(const FBattleAction& Action, FBattleAction& OutAction) const
{
	ABattleSimulationCharacter* SimulationAttacker = GetSimulationCharacter(Action.Attacker.Get());
	if (!IsValid(SimulationAttacker) || !IsValid(Action.Card.Get()))
		return false;
	OutAction = Action;
	OutAction.Attacker = SimulationAttacker;
	return true;
}

void UBattleSimulationWorldRuntime::HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FTargetingResult& TargetingResult)
{
	(void)Entry;
	(void)EntryIndex;
	if (IsValid(SimulationManager.Get()))
		SimulationManager->NotifySimulationWorldExecutionStarted(this, Action, TargetingResult);
}

void UBattleSimulationWorldRuntime::HandleSimulationActionFinished()
{
	if (SimulationState == EBattleSimulationState::ExecutingFirstAction)
	{
		const bool bPlayerFirst = PreparedPlayerAction.Speed >= PreparedEnemyAction.Speed;
		const FBattleAction& SecondAction = bPlayerFirst ? PreparedEnemyAction : PreparedPlayerAction;
		SimulationState = EBattleSimulationState::ExecutingSecondAction;
		if (!ActionExecutor || !ActionExecutor->ExecuteBattleAction(SecondAction))
			FailCurrentExchange();
		return;
	}
	if (SimulationState == EBattleSimulationState::ExecutingSecondAction)
		FinishCurrentExchange();
}

void UBattleSimulationWorldRuntime::FinishCurrentExchange()
{
	const int32 FinishedExchangeIndex = PreparedPlayerAction.ExchangeIndex;
	const ABattleManager* BattleManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleManager() : nullptr;
	const bool bSimulationCompleted = !IsValid(BattleManager) || FinishedExchangeIndex + 1 >= BattleManager->GetMaxExchangeCount();

	ClearPreparedActions();
	SimulationState = bSimulationCompleted ? EBattleSimulationState::Completed : EBattleSimulationState::Ready;
	if (IsValid(SimulationManager.Get()))
		SimulationManager->NotifySimulationWorldExchangeCompleted(this, FinishedExchangeIndex, true);
}

void UBattleSimulationWorldRuntime::ClearPreparedActions()
{
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
}

void UBattleSimulationWorldRuntime::DestroySimulationRuntime()
{
	if (ActionExecutor)
	{
		ActionExecutor->OnBattleActionCompleted.Unbind();
		ActionExecutor->OnBattleExecutionStarted.Unbind();
		ActionExecutor->Stop();
		ActionExecutor = nullptr;
	}
	ABattleGridManager* GridManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleGridManager() : nullptr;
	if (IsValid(GridManager))
		GridManager->RemoveWorldState(WorldType);
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value.Get()))
			Pair.Value->Destroy();
	}
	SimulationCharacterMap.Empty();
}
