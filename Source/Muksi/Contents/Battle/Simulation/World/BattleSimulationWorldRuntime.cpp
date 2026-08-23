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
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"

bool UBattleSimulationWorldRuntime::Initialize(ABattleSimulationManager* InSimulationManager, EBattleSimulationWorldType InWorldType)
{
	if (!IsValid(InSimulationManager) || !BattleSimulationWorld::UsesSimulationRuntime(InWorldType)) return false;
	SimulationManager = InSimulationManager;
	WorldType = InWorldType;
	return true;
}

void UBattleSimulationWorldRuntime::Shutdown()
{
	ExchangeFinishedDelegate.Clear();
	DestroySimulationRuntime();
	SimulationManager = nullptr;
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetSimulationState(EBattleSimulationState::Idle);
}

void UBattleSimulationWorldRuntime::BeginDestroy()
{
	Shutdown();
	Super::BeginDestroy();
}

bool UBattleSimulationWorldRuntime::PrepareSimulationRuntime(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(SourceGridManager) || SourceCharacters.IsEmpty()) return false;
	if (IsSimulationRuntimeReady()) return true;
	DestroySimulationRuntime();
	if (!CreateSimulationCharacters(SourceCharacters) || !CreateSimulationExecutionEnvironment(SourceGridManager))
	{
		ResetSimulationRuntime();
		return false;
	}
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetCharactersVisible(false);
	SetSimulationState(EBattleSimulationState::Idle);
	return true;
}

bool UBattleSimulationWorldRuntime::ResetFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(InSourceGridManager) || SourceCharacters.IsEmpty() || !IsSimulationRuntimeReady()) return false;
	if (ActionExecutor && ActionExecutor->IsRunning()) return false;
	if (!CanReuseSimulationRuntime(InSourceGridManager, SourceCharacters) || !ResetSimulationRuntimeFromActualBattleState(InSourceGridManager, SourceCharacters))
	{
		SetSimulationState(EBattleSimulationState::Idle);
		return false;
	}
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetSimulationState(EBattleSimulationState::Ready);
	return true;
}

void UBattleSimulationWorldRuntime::SetCharactersVisible(bool bVisible)
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value.Get())) Pair.Value->SetActorHiddenInGame(!bVisible);
	}
}

bool UBattleSimulationWorldRuntime::PrepareExchange(int32 ExchangeIndex, const FBattleAction& PlayerAction, const FBattleAction& EnemyAction)
{
	if (SimulationState != EBattleSimulationState::Ready) return false;
	FBattleAction PlayerSimulationAction = PlayerAction;
	PlayerSimulationAction.ExchangeIndex = ExchangeIndex;
	PlayerSimulationAction.bPlayerAction = true;
	FBattleAction EnemySimulationAction = EnemyAction;
	EnemySimulationAction.ExchangeIndex = ExchangeIndex;
	EnemySimulationAction.bPlayerAction = false;
	FBattleAction NewPlayerAction;
	FBattleAction NewEnemyAction;
	if (!BuildSimulationAction(PlayerSimulationAction, NewPlayerAction)) return false;
	if (!BuildSimulationAction(EnemySimulationAction, NewEnemyAction)) return false;
	this->PreparedPlayerAction = MoveTemp(NewPlayerAction);
	this->PreparedEnemyAction = MoveTemp(NewEnemyAction);
	SetSimulationState(EBattleSimulationState::Prepared);
	return true;
}

bool UBattleSimulationWorldRuntime::ExecuteCurrentExchange()
{
	if (SimulationState != EBattleSimulationState::Prepared) return false;
	return TryExecuteCurrentExchange();
}

void UBattleSimulationWorldRuntime::StopSimulation()
{
	ClearSimulationActionPresentation();
	DestroySimulationRuntime();
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetSimulationState(EBattleSimulationState::Completed);
}

bool UBattleSimulationWorldRuntime::IsSimulationRunning() const
{
	return SimulationState != EBattleSimulationState::Idle && SimulationState != EBattleSimulationState::Completed;
}

bool UBattleSimulationWorldRuntime::IsSimulationRuntimeReady() const
{
	return IsValid(BattleGridManager.Get()) && BattleGridManager->HasWorldState(WorldType) && IsValid(ActionExecutor.Get()) && !SimulationCharacterMap.IsEmpty();
}

ABattleSimulationCharacter* UBattleSimulationWorldRuntime::GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter)) return nullptr;
	const TObjectPtr<ABattleSimulationCharacter>* FoundCharacter = SimulationCharacterMap.Find(SourceCharacter);
	return FoundCharacter ? FoundCharacter->Get() : nullptr;
}

ABattleCharacterBase* UBattleSimulationWorldRuntime::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	return IsValid(SimulationCharacter) ? SimulationCharacter->GetSourceCharacter() : nullptr;
}

bool UBattleSimulationWorldRuntime::CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	UWorld* World = IsValid(SimulationManager.Get()) ? SimulationManager->GetWorld() : nullptr;
	TSubclassOf<ABattleSimulationCharacter> CharacterClass;
	if (IsValid(SimulationManager.Get())) CharacterClass = SimulationManager->GetSimulationCharacterClass();
	if (!World || !CharacterClass || SourceCharacters.IsEmpty()) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter)) return false;
		ABattleSimulationCharacter* SimulationCharacter = World->SpawnActor<ABattleSimulationCharacter>(CharacterClass, SourceCharacter->GetActorTransform());
		if (!SimulationCharacter) return false;
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? SimulationManager->GetPlayerSimulationMaterial() : SimulationManager->GetEnemySimulationMaterial();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
		SimulationCharacterMap.Add(SourceCharacter, SimulationCharacter);
	}
	return SimulationCharacterMap.Num() == SourceCharacters.Num();
}

bool UBattleSimulationWorldRuntime::CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager)
{
	if (!IsValid(InSourceGridManager)) return false;
	BattleGridManager = InSourceGridManager;
	TArray<ABattleCharacterBase*> SourceCharacters;
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap) SourceCharacters.Add(Pair.Key.Get());
	if (!RebuildWorldGridState(SourceCharacters)) return false;
	ActionExecutor = NewObject<UBattleActionExecutor>(this);
	ABattleManager* OwningBattleManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleManager() : nullptr;
	if (!ActionExecutor || !ActionExecutor->Initialize(OwningBattleManager, BattleGridManager.Get(), WorldType)) return false;
	ActionExecutor->FinishedDelegate.AddUObject(this, &UBattleSimulationWorldRuntime::HandleSimulationSequenceFinished);
	ActionExecutor->EntryStartedDelegate.AddUObject(this, &UBattleSimulationWorldRuntime::HandleSimulationExecutionStarted);
	return true;
}

bool UBattleSimulationWorldRuntime::RebuildWorldGridState(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!BattleGridManager || SourceCharacters.IsEmpty()) return false;
	TArray<AActor*> Sources;
	TArray<AActor*> Replacements;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(SourceCharacter);
		if (!IsValid(SourceCharacter) || !IsValid(SimulationCharacter)) return false;
		Sources.Add(SourceCharacter);
		Replacements.Add(SimulationCharacter);
	}
	return BattleGridManager->InitializeWorldStateFromActual(WorldType, Sources, Replacements);
}

bool UBattleSimulationWorldRuntime::CanReuseSimulationRuntime(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters) const
{
	if (!IsValid(InSourceGridManager) || InSourceGridManager != BattleGridManager.Get() || !IsValid(ActionExecutor.Get())) return false;
	if (ActionExecutor->IsRunning() || SimulationCharacterMap.Num() != SourceCharacters.Num()) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		const TObjectPtr<ABattleSimulationCharacter>* SimulationCharacter = SimulationCharacterMap.Find(SourceCharacter);
		if (!IsValid(SourceCharacter) || !SimulationCharacter || !IsValid(SimulationCharacter->Get())) return false;
	}
	return true;
}

bool UBattleSimulationWorldRuntime::ResetSimulationRuntimeFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (InSourceGridManager != BattleGridManager.Get() || !IsValid(SimulationManager.Get())) return false;
	ClearSimulationActionPresentation();
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(SourceCharacter);
		if (!IsValid(SimulationCharacter)) return false;
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? SimulationManager->GetPlayerSimulationMaterial() : SimulationManager->GetEnemySimulationMaterial();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
	}
	if (!RebuildWorldGridState(SourceCharacters)) return false;
	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();
	return true;
}

bool UBattleSimulationWorldRuntime::TryExecuteCurrentExchange()
{
	const bool bPlayerFirst = PreparedPlayerAction.Speed >= PreparedEnemyAction.Speed;
	const FBattleAction& FirstAction = bPlayerFirst ? PreparedPlayerAction : PreparedEnemyAction;
	SetSimulationState(EBattleSimulationState::ExecutingFirstAction);
	return ExecuteSimulationAction(FirstAction);
}

bool UBattleSimulationWorldRuntime::BuildSimulationAction(const FBattleAction& Action, FBattleAction& OutAction) const
{
	ABattleSimulationCharacter* SimulationAttacker = GetSimulationCharacter(Action.Attacker.Get());
	if (!IsValid(SimulationAttacker) || !IsValid(Action.Card.Get())) return false;
	OutAction = Action;
	OutAction.Attacker = SimulationAttacker;
	return true;
}

bool UBattleSimulationWorldRuntime::ExecuteSimulationAction(const FBattleAction& Action)
{
	if (!ActionExecutor) return false;
	ClearSimulationActionPresentation();
	return ActionExecutor->ExecuteAction(Action);
}

void UBattleSimulationWorldRuntime::ClearSimulationActionPresentation()
{
	if (IsValid(SimulationManager.Get())) SimulationManager->ClearSimulationExecutionPresentation(this);
}

void UBattleSimulationWorldRuntime::HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting)
{
	(void)Entry;
	(void)EntryIndex;
	if (IsValid(SimulationManager.Get())) SimulationManager->PresentSimulationExecution(this, Action, ResolvedTargeting);
}

void UBattleSimulationWorldRuntime::HandleSimulationSequenceFinished()
{
	ClearSimulationActionPresentation();
	if (SimulationState == EBattleSimulationState::ExecutingFirstAction)
	{
		const bool bPlayerFirst = PreparedPlayerAction.Speed >= PreparedEnemyAction.Speed;
		const FBattleAction& SecondAction = bPlayerFirst ? PreparedEnemyAction : PreparedPlayerAction;
		SetSimulationState(EBattleSimulationState::ExecutingSecondAction);
		if (!ExecuteSimulationAction(SecondAction)) StopSimulation();
		return;
	}
	if (SimulationState == EBattleSimulationState::ExecutingSecondAction) FinishCurrentExchange();
}

void UBattleSimulationWorldRuntime::FinishCurrentExchange()
{
	const int32 FinishedExchangeIndex = PreparedPlayerAction.ExchangeIndex;
	const int32 NextExchangeIndex = FinishedExchangeIndex + 1;
	const bool bSimulationCompleted = NextExchangeIndex >= GetMaxExchangeCount();
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetSimulationState(bSimulationCompleted ? EBattleSimulationState::Completed : EBattleSimulationState::Ready);
	ExchangeFinishedDelegate.Broadcast(this, FinishedExchangeIndex);
}

void UBattleSimulationWorldRuntime::DestroySimulationRuntime()
{
	ClearSimulationActionPresentation();
	if (ActionExecutor)
	{
		ActionExecutor->FinishedDelegate.RemoveAll(this);
		ActionExecutor->EntryStartedDelegate.RemoveAll(this);
		ActionExecutor->Stop();
		ActionExecutor = nullptr;
	}
	if (BattleGridManager)
	{
		BattleGridManager->RemoveWorldState(WorldType);
		BattleGridManager = nullptr;
	}
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value.Get())) Pair.Value->Destroy();
	}
	SimulationCharacterMap.Empty();
}

void UBattleSimulationWorldRuntime::ResetSimulationRuntime()
{
	DestroySimulationRuntime();
	PreparedPlayerAction = FBattleAction();
	PreparedEnemyAction = FBattleAction();
	SetSimulationState(EBattleSimulationState::Idle);
}

void UBattleSimulationWorldRuntime::SetSimulationState(EBattleSimulationState NewState)
{
	if (SimulationState == NewState) return;
	SimulationState = NewState;
}

int32 UBattleSimulationWorldRuntime::GetMaxExchangeCount() const
{
	const ABattleManager* OwningBattleManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleManager() : nullptr;
	return IsValid(OwningBattleManager) ? OwningBattleManager->GetMaxExchangeCount() : 0;
}
