#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldManager.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

ABattleSimulationWorldManager::ABattleSimulationWorldManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABattleSimulationWorldManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroySimulationRuntime();
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationWorldManager::InitializeWorld(const FBattleSimulationWorldPolicy& InWorldPolicy, TSubclassOf<ABattleSimulationCharacter> InSimulationCharacterClass, UMaterialInterface* InPlayerSimulationMaterial, UMaterialInterface* InEnemySimulationMaterial)
{
	if (!InWorldPolicy.UsesSimulationRuntime() || !InSimulationCharacterClass)
		return false;

	WorldPolicy = InWorldPolicy;
	SimulationCharacterClass = InSimulationCharacterClass;
	PlayerSimulationMaterial = InPlayerSimulationMaterial;
	EnemySimulationMaterial = InEnemySimulationMaterial;

	if (!SimulationTargetingPresentationController)
		SimulationTargetingPresentationController = NewObject<UTargetingPresentationController>(this);

	return IsValid(SimulationTargetingPresentationController);
}

bool ABattleSimulationWorldManager::PrepareSimulationRuntime(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(SourceGridManager) || SourceCharacters.IsEmpty()) return false;
	if (IsSimulationRuntimeReady()) return true;

	DestroySimulationRuntime();
	if (!CreateSimulationCharacters(SourceCharacters) || !CreateSimulationExecutionEnvironment(SourceGridManager))
	{
		ResetSimulationRuntime();
		return false;
	}

	CurrentExchange.Reset(INDEX_NONE);
	SetWorldVisible(false);
	SetSimulationState(EBattleSimulationState::Idle);
	return true;
}

bool ABattleSimulationWorldManager::ResetFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(InSourceGridManager) || SourceCharacters.IsEmpty() || !IsSimulationRuntimeReady())
		return false;
	if (SimulationSequenceManager && SimulationSequenceManager->IsSequenceRunning())
		return false;

	SetSimulationState(EBattleSimulationState::Starting);
	if (!CanReuseSimulationRuntime(InSourceGridManager, SourceCharacters) || !ResetSimulationRuntimeFromActualBattleState(InSourceGridManager, SourceCharacters))
	{
		SetSimulationState(EBattleSimulationState::Idle);
		return false;
	}
	CurrentExchange.Reset(0);
	SetWorldVisible(bWorldVisible);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
	return true;
}

void ABattleSimulationWorldManager::SetWorldVisible(bool bVisible)
{
	const bool bWasWorldVisible = bWorldVisible;

	if (!bVisible && bWasWorldVisible) ClearRuntimeSimulationPreview();
	if (!bVisible && bWasWorldVisible && SimulationGridManager) SimulationGridManager->AllClearGridHovered();
	if (!bVisible && bWasWorldVisible && SimulationGridManager) SimulationGridManager->AllClearExchangeIndicator();
	bWorldVisible = bVisible;

	if (SimulationGridManager) SimulationGridManager->SetTilePresentationEnabled(bWorldVisible);

	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value)) Pair.Value->SetActorHiddenInGame(!bWorldVisible);
	}

	if (SimulationGridManager) SimulationGridManager->SetActorHiddenInGame(!bWorldVisible);
	if (bWorldVisible && bHasCachedPresentation) RefreshSimulationTargetingPresentation(CachedPresentationAction, CachedPresentationResolvedTargeting);
}

bool ABattleSimulationWorldManager::PrepareExchange(int32 ExchangeIndex, const FBattleAction& PlayerAction, const FBattleAction& EnemyAction)
{
	if (!IsSimulationRunning()) return false;
	CurrentExchange.Reset(ExchangeIndex);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
	if (!SetEnemyAction(EnemyAction) || !SetPlayerAction(PlayerAction))
	{
		CurrentExchange.Reset(ExchangeIndex);
		SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
		return false;
	}
	return true;
}

bool ABattleSimulationWorldManager::ExecuteCurrentExchange()
{
	if (!IsSimulationRunning()) return false;
	if (SimulationState != EBattleSimulationState::RevealingEnemyCard) return false;
	return TryExecuteCurrentExchange();
}

void ABattleSimulationWorldManager::StopSimulation()
{
	ClearRuntimeSimulationPreview();
	DestroySimulationRuntime();
	CurrentExchange.Reset(INDEX_NONE);
	SetSimulationState(EBattleSimulationState::Completed);
}

bool ABattleSimulationWorldManager::IsSimulationRunning() const
{
	return SimulationState != EBattleSimulationState::Idle && SimulationState != EBattleSimulationState::Completed;
}

bool ABattleSimulationWorldManager::IsSimulationRuntimeReady() const
{
	return IsValid(SimulationGridManager) && IsValid(SimulationSequenceManager) && !SimulationCharacterMap.IsEmpty();
}

ABattleSimulationCharacter* ABattleSimulationWorldManager::GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter)) return nullptr;
	const TObjectPtr<ABattleSimulationCharacter>* FoundCharacter = SimulationCharacterMap.Find(SourceCharacter);
	return FoundCharacter ? FoundCharacter->Get() : nullptr;
}

ABattleCharacterBase* ABattleSimulationWorldManager::GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const
{
	return IsValid(SimulationCharacter) ? SimulationCharacter->GetSourceCharacter() : nullptr;
}

bool ABattleSimulationWorldManager::SetPlayerAction(const FBattleAction& PlayerAction)
{
	UMuksiBattleCardDataAsset* ExecutionOverride = GetExecutionOverride(PlayerAction, WorldPolicy.PlayerKnowledge);
	if (!IsSimulationRunning() || CurrentExchange.bHasPlayerAction || !IsValid(PlayerAction.Attacker) || !IsValid(PlayerAction.Card) || (ExecutionOverride && !IsValid(ExecutionOverride))) return false;
	CurrentExchange.SetPlayerAction(PlayerAction, ExecutionOverride);
	SetSimulationState(CurrentExchange.CanResolveActionOrder() ? EBattleSimulationState::RevealingEnemyCard : EBattleSimulationState::WaitingForPlayerTargeting);
	return true;
}

bool ABattleSimulationWorldManager::SetEnemyAction(const FBattleAction& EnemyAction)
{
	UMuksiBattleCardDataAsset* ExecutionOverride = GetExecutionOverride(EnemyAction, WorldPolicy.EnemyKnowledge);
	if (!IsSimulationRunning() || CurrentExchange.bHasEnemyAction || !IsValid(EnemyAction.Attacker) || !IsValid(EnemyAction.Card) || (ExecutionOverride && !IsValid(ExecutionOverride))) return false;
	CurrentExchange.SetEnemyAction(EnemyAction, ExecutionOverride);
	SetSimulationState(CurrentExchange.CanResolveActionOrder() ? EBattleSimulationState::RevealingEnemyCard : EBattleSimulationState::WaitingForPlayerTargeting);
	return true;
}

UMuksiBattleCardDataAsset* ABattleSimulationWorldManager::GetExecutionOverride(const FBattleAction& Action, EBattleSimulationKnowledge Knowledge) const
{
	if (Knowledge == EBattleSimulationKnowledge::Actual || !IsValid(Action.Card)) return nullptr;
	UMuksiBattleCardDataAsset* DeceivedCard = Action.Card->GetDeceivedCard();
	return IsValid(DeceivedCard) ? DeceivedCard : nullptr;
}

bool ABattleSimulationWorldManager::CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	UWorld* World = GetWorld();
	if (!World || !SimulationCharacterClass || SourceCharacters.IsEmpty()) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter)) return false;
		ABattleSimulationCharacter* SimulationCharacter = World->SpawnActor<ABattleSimulationCharacter>(SimulationCharacterClass, SourceCharacter->GetActorTransform());
		if (!SimulationCharacter) return false;
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? PlayerSimulationMaterial.Get() : EnemySimulationMaterial.Get();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
		SimulationCharacterMap.Add(SourceCharacter, SimulationCharacter);
	}
	return SimulationCharacterMap.Num() == SourceCharacters.Num();
}

bool ABattleSimulationWorldManager::CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager)
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(InSourceGridManager)) return false;
	SimulationGridManager = World->SpawnActorDeferred<ABattleGridManager>(ABattleGridManager::StaticClass(), InSourceGridManager->GetActorTransform(), this);
	if (!SimulationGridManager)
	{
		return false;
	}
	SimulationGridManager->SetGridGenerationEnabled(false);
	UGameplayStatics::FinishSpawningActor(SimulationGridManager, InSourceGridManager->GetActorTransform());
	if (!SimulationGridManager->InitializeRuntimeGridFromSource(InSourceGridManager))
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		return false;
	}
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (SimulationGridManager->ReplaceGridActor(Pair.Key.Get(), Pair.Value.Get())) continue;
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		return false;
	}
	SimulationSequenceManager = World->SpawnActorDeferred<ABattleSequenceManager>(ABattleSequenceManager::StaticClass(), FTransform::Identity, this);
	if (!SimulationSequenceManager)
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		return false;
	}
	UGameplayStatics::FinishSpawningActor(SimulationSequenceManager, FTransform::Identity);
	SimulationSequenceManager->BattleGridManager = SimulationGridManager;
	SimulationSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleSimulationWorldManager::HandleSimulationSequenceFinished);
	SimulationSequenceManager->OnExecutionEntryStarted.AddUObject(this, &ABattleSimulationWorldManager::HandleSimulationExecutionStarted);
	SimulationTargetingPresentationController->Initialize(SimulationGridManager);
	return true;
}

bool ABattleSimulationWorldManager::CanReuseSimulationRuntime(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters) const
{
	if (!IsValid(InSourceGridManager) || !IsValid(SimulationGridManager) || !IsValid(SimulationSequenceManager)) return false;
	if (SimulationSequenceManager->IsSequenceRunning() || SimulationCharacterMap.Num() != SourceCharacters.Num()) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		const TObjectPtr<ABattleSimulationCharacter>* SimulationCharacter = SimulationCharacterMap.Find(SourceCharacter);
		if (!IsValid(SourceCharacter) || !SimulationCharacter || !IsValid(SimulationCharacter->Get())) return false;
	}
	return true;
}

bool ABattleSimulationWorldManager::ResetSimulationRuntimeFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	bHasCachedPresentation = false;
	CachedPresentationAction = FBattleAction();
	CachedPresentationResolvedTargeting = FResolvedTargeting();
	ClearRuntimeSimulationPreview();
	if (!SimulationGridManager->InitializeRuntimeGridFromSource(InSourceGridManager)) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(SourceCharacter);
		if (!IsValid(SimulationCharacter)) return false;
		UMaterialInterface* TeamMaterial = SourceCharacter->IsA<ABattleCharacter_Player>() ? PlayerSimulationMaterial.Get() : EnemySimulationMaterial.Get();
		SimulationCharacter->InitializeFromCharacter(SourceCharacter, TeamMaterial);
		if (!SimulationGridManager->ReplaceGridActor(SourceCharacter, SimulationCharacter)) return false;
	}
	SimulationGridManager->AllClearGridHovered();
	SimulationGridManager->AllClearExchangeIndicator();
	return true;
}

bool ABattleSimulationWorldManager::TryExecuteCurrentExchange()
{
	if (!CurrentExchange.CanResolveActionOrder()) return false;
	SetSimulationState(EBattleSimulationState::ResolvingActionOrder);
	if (!CurrentExchange.ResolveActionOrder()) return false;
	SetSimulationState(EBattleSimulationState::ExecutingFirstAction);
	return ExecuteSimulationAction(CurrentExchange.GetFirstAction());
}

bool ABattleSimulationWorldManager::ExecuteSimulationAction(const FBattleSimulationActionPlan& ActionPlan)
{
	if (!SimulationSequenceManager) return false;
	FBattleSequenceRequest Request;
	if (!BuildSimulationSequenceRequest(ActionPlan, Request)) return false;
	ClearSimulationActionPresentation();
	if (SimulationSequenceManager->StartSequenceWithRequest(Request)) return true;
	ClearSimulationActionPresentation();
	return false;
}

bool ABattleSimulationWorldManager::BuildSimulationSequenceRequest(const FBattleSimulationActionPlan& ActionPlan, FBattleSequenceRequest& OutRequest) const
{
	ABattleSimulationCharacter* SimulationAttacker = GetSimulationCharacter(ActionPlan.SequenceAction.Attacker);
	UMuksiBattleCardDataAsset* SimulationCard = ActionPlan.GetSimulationCard();
	if (!IsValid(SimulationAttacker) || !IsValid(SimulationCard)) return false;
	OutRequest.Action = ActionPlan.SequenceAction;
	OutRequest.Action.Attacker = SimulationAttacker;
	OutRequest.ExecutionMode = EBattleExecutionMode::Simulation;
	OutRequest.ExecutionCardOverride = SimulationCard;
	return true;
}

void ABattleSimulationWorldManager::ClearSimulationActionPresentation()
{
	bHasCachedPresentation = false;
	if (!bWorldVisible) return;
	ClearRuntimeSimulationPreview();
	if (!SimulationGridManager) return;
	SimulationGridManager->AllClearGridHovered();
	SimulationGridManager->AllClearExchangeIndicator();
}

void ABattleSimulationWorldManager::RefreshSimulationTargetingPresentation(const FBattleAction& Action, const FResolvedTargeting& ExecutionResolvedTargeting)
{
	if (!bWorldVisible) return;
	ClearRuntimeSimulationPreview();
	if (!SimulationGridManager || !IsValid(Action.Card)) return;
	SimulationGridManager->AllClearGridHovered();
	SimulationGridManager->AllClearExchangeIndicator();
	ABattleCharacterBase* RuntimeAttacker = Action.Attacker;
	if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(Action.Attacker)) RuntimeAttacker = SimulationCharacter;
	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData) continue;
		FResolvedTargeting StepResolvedTargeting;
		if (StepIndex == StepCount - 1) StepResolvedTargeting = ExecutionResolvedTargeting;
		else if (!ResolveSimulationActionTargetingThroughStep(Action, StepIndex, StepResolvedTargeting))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RuntimeTargetingPresentation] Simulation resolve failed. Step=%d"), StepIndex);
			continue;
		}
		const FTargetingPhasePresentationSettings& PresentationSettings = StepData->AdvancedSettings.Presentation.SimulationPhase;
		if (PresentationSettings.bShowIndicator)
		{
			TArray<FHexOffsetCoord> StepIndicatorCoords = StepResolvedTargeting.AffectedCoords;
			if (StepIndicatorCoords.IsEmpty())
			{
				const FTargetingStepResult* StepResult = StepResolvedTargeting.GetStep(StepIndex);
				if (StepResult && StepResult->HasSelectedCoord()) StepIndicatorCoords.Add(StepResult->SelectedCoord);
			}
			for (const FHexOffsetCoord& Coord : StepIndicatorCoords) IndicatorCoords.AddUnique(Coord);
		}
		const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview || PresentationSettings.bShowPathPreview || PresentationSettings.bShowAreaPreview;
		if (bShowAnyPreview && SimulationTargetingPresentationController) SimulationTargetingPresentationController->AddResolvedStepPreview(RuntimeAttacker, Action.Card->TargetingData, StepResolvedTargeting, StepIndex, PresentationSettings, !Action.bPlayerAction);
	}
	if (!IndicatorCoords.IsEmpty()) SimulationGridManager->SetExchangeIndicator(Action.Card->CardTypeInfo, IndicatorCoords, !Action.bPlayerAction);
}

bool ABattleSimulationWorldManager::ResolveSimulationActionTargetingThroughStep(const FBattleAction& Action, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting) const
{
	FBattleAction RuntimeAction = Action;
	if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(Action.Attacker)) RuntimeAction.Attacker = SimulationCharacter;
	return FBattleTargetResolver::ResolveActionThroughStep(RuntimeAction, SimulationGridManager, LastStepIndex, OutResolvedTargeting);
}

void ABattleSimulationWorldManager::ClearRuntimeSimulationPreview()
{
	if (SimulationTargetingPresentationController) SimulationTargetingPresentationController->ClearExecutionPreview();
}

void ABattleSimulationWorldManager::HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting)
{
	(void)Entry;
	(void)EntryIndex;
	CachedPresentationAction = Action;
	CachedPresentationResolvedTargeting = ResolvedTargeting;
	bHasCachedPresentation = true;
	if (!bWorldVisible) return;
	RefreshSimulationTargetingPresentation(Action, ResolvedTargeting);
}

void ABattleSimulationWorldManager::HandleSimulationSequenceFinished()
{
	ClearSimulationActionPresentation();
	if (SimulationState == EBattleSimulationState::ExecutingFirstAction)
	{
		SetSimulationState(EBattleSimulationState::ExecutingSecondAction);
		if (!ExecuteSimulationAction(CurrentExchange.GetSecondAction())) StopSimulation();
		return;
	}
	if (SimulationState == EBattleSimulationState::ExecutingSecondAction) FinishCurrentExchange();
}

void ABattleSimulationWorldManager::FinishCurrentExchange()
{
	SetSimulationState(EBattleSimulationState::FinishingExchange);
	const FBattleSimulationExchange FinishedExchange = CurrentExchange;
	const int32 FinishedExchangeIndex = FinishedExchange.ExchangeIndex;
	const int32 NextExchangeIndex = FinishedExchangeIndex + 1;
	const bool bSimulationCompleted = NextExchangeIndex >= GetMaxExchangeCount();
	if (bSimulationCompleted)
	{
		SetSimulationState(EBattleSimulationState::FinishingSimulation);
		SetSimulationState(EBattleSimulationState::Completed);
	}
	else
	{
		CurrentExchange.Reset(NextExchangeIndex);
		SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
	}
	ExchangeFinishedDelegate.Broadcast(this, FinishedExchangeIndex, bSimulationCompleted, FinishedExchange);
}

void ABattleSimulationWorldManager::DestroySimulationRuntime()
{
	bHasCachedPresentation = false;
	CachedPresentationAction = FBattleAction();
	CachedPresentationResolvedTargeting = FResolvedTargeting();
	ClearRuntimeSimulationPreview();
	if (SimulationSequenceManager)
	{
		SimulationSequenceManager->OnSequenceFinished.RemoveAll(this);
		SimulationSequenceManager->OnExecutionEntryStarted.RemoveAll(this);
		SimulationSequenceManager->Destroy();
		SimulationSequenceManager = nullptr;
	}
	if (SimulationGridManager)
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
	}
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value)) Pair.Value->Destroy();
	}
	SimulationCharacterMap.Empty();
}

void ABattleSimulationWorldManager::ResetSimulationRuntime()
{
	DestroySimulationRuntime();
	CurrentExchange.Reset(INDEX_NONE);
	SetSimulationState(EBattleSimulationState::Idle);
}

void ABattleSimulationWorldManager::SetSimulationState(EBattleSimulationState NewState)
{
	if (SimulationState == NewState) return;
	SimulationState = NewState;
}

int32 ABattleSimulationWorldManager::GetMaxExchangeCount() const
{
	const ABattleSimulationManager* SimulationManager = Cast<ABattleSimulationManager>(GetOwner());
	const ABattleManager* OwningBattleManager = IsValid(SimulationManager) ? SimulationManager->GetBattleManager() : nullptr;
	return IsValid(OwningBattleManager) ? OwningBattleManager->GetMaxExchangeCount() : 0;
}
