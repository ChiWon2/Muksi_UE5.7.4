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
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
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
	BattleManager = nullptr;
	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationWorldManager::InitializeWorld(const FBattleSimulationWorldPolicy& InWorldPolicy, ABattleManager* InBattleManager, int32 InMaxExchangeCount, TSubclassOf<ABattleSimulationCharacter> InSimulationCharacterClass, UMaterialInterface* InPlayerSimulationMaterial, UMaterialInterface* InEnemySimulationMaterial)
{
	if (!InWorldPolicy.UsesSimulationRuntime() || !IsValid(InBattleManager) || !InSimulationCharacterClass) return false;
	WorldPolicy = InWorldPolicy;
	BattleManager = InBattleManager;
	MaxExchangeCount = InMaxExchangeCount;
	SimulationCharacterClass = InSimulationCharacterClass;
	PlayerSimulationMaterial = InPlayerSimulationMaterial;
	EnemySimulationMaterial = InEnemySimulationMaterial;
	if (!SimulationTargetingPresentationController) SimulationTargetingPresentationController = NewObject<UTargetingPresentationController>(this);
	return IsValid(SimulationTargetingPresentationController);
}

bool ABattleSimulationWorldManager::ResetFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(BattleManager)) return false;
	ResetSimulationRuntime();
	SetSimulationState(EBattleSimulationState::Starting);
	if (!CreateSimulationCharacters(SourceCharacters) || !CreateSimulationExecutionEnvironment(InSourceGridManager))
	{
		ResetSimulationRuntime();
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

void ABattleSimulationWorldManager::NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (NewPhase != EBattlePhase::ExchangeStart && NewPhase != EBattlePhase::ExchangeEnd) return;
	for (ABattleSimulationCharacter* SimulationCharacter : SimulationCharacterOrder)
	{
		if (!IsValid(SimulationCharacter)) continue;
		if (UCharacterPassiveComponent* PassiveComponent = SimulationCharacter->GetPassiveComponent()) PassiveComponent->NotifyBattlePhaseChanged(OldPhase, NewPhase);
		if (UMuksiStatusEffectComponent* StatusEffectComponent = SimulationCharacter->GetStatusEffectComponent()) StatusEffectComponent->NotifyBattlePhaseChanged(OldPhase, NewPhase);
	}
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
		SimulationCharacterOrder.Add(SimulationCharacter);
	}
	return SimulationCharacterMap.Num() == SourceCharacters.Num();
}

bool ABattleSimulationWorldManager::CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager)
{
	UWorld* World = GetWorld();
	if (!World || !IsValid(InSourceGridManager)) return false;
	SourceGridManager = InSourceGridManager;
	SimulationGridManager = World->SpawnActorDeferred<ABattleGridManager>(ABattleGridManager::StaticClass(), SourceGridManager->GetActorTransform(), this);
	if (!SimulationGridManager)
	{
		SourceGridManager = nullptr;
		return false;
	}
	SimulationGridManager->SetWorldManagerRegistrationEnabled(false);
	SimulationGridManager->SetGridGenerationEnabled(false);
	UGameplayStatics::FinishSpawningActor(SimulationGridManager, SourceGridManager->GetActorTransform());
	if (!SimulationGridManager->InitializeRuntimeGridFromSource(SourceGridManager))
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		SourceGridManager = nullptr;
		return false;
	}
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (SimulationGridManager->ReplaceGridActor(Pair.Key.Get(), Pair.Value.Get())) continue;
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		SourceGridManager = nullptr;
		return false;
	}
	SimulationSequenceManager = World->SpawnActorDeferred<ABattleSequenceManager>(ABattleSequenceManager::StaticClass(), FTransform::Identity, this);
	if (!SimulationSequenceManager)
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
		SourceGridManager = nullptr;
		return false;
	}
	SimulationSequenceManager->SetWorldManagerRegistrationEnabled(false);
	UGameplayStatics::FinishSpawningActor(SimulationSequenceManager, FTransform::Identity);
	SimulationSequenceManager->BattleGridManager = SimulationGridManager;
	SimulationSequenceManager->BattleActionStartDelegate.AddUObject(this, &ABattleSimulationWorldManager::HandleSimulationBattleActionStart);
	SimulationSequenceManager->OnSequenceFinished.AddUObject(this, &ABattleSimulationWorldManager::HandleSimulationSequenceFinished);
	SimulationSequenceManager->OnExecutionEntryStarted.AddUObject(this, &ABattleSimulationWorldManager::HandleSimulationExecutionStarted);
	SimulationTargetingPresentationController->Initialize(SimulationGridManager);
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
	HandleSimulationActionStarted(ActionPlan.SequenceAction);
	if (SimulationSequenceManager->StartSequenceWithRequest(Request)) return true;
	HandleSimulationActionFinished();
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

void ABattleSimulationWorldManager::HandleSimulationActionStarted(const FBattleAction& Action)
{
	(void)Action;
	bHasCachedPresentation = false;
	if (!bWorldVisible) return;
	ClearRuntimeSimulationPreview();
	if (!SimulationGridManager) return;
	SimulationGridManager->AllClearGridHovered();
	SimulationGridManager->AllClearExchangeIndicator();
}

void ABattleSimulationWorldManager::HandleSimulationActionFinished()
{
	bHasCachedPresentation = false;
	if (!bWorldVisible) return;
	ClearRuntimeSimulationPreview();
	if (!SimulationGridManager) return;
	SimulationGridManager->AllClearGridHovered();
	SimulationGridManager->AllClearExchangeIndicator();
}

void ABattleSimulationWorldManager::HandleSimulationBattleActionStart(const FBattleAction& BattleAction)
{
	FBattleAction WorldBattleAction = BattleAction;
	UMuksiBattleCardDataAsset* ExecutionOverride = GetExecutionOverride(WorldBattleAction, WorldPolicy.GetKnowledge(WorldBattleAction.bPlayerAction));
	if (IsValid(ExecutionOverride)) WorldBattleAction.Card = ExecutionOverride;
	for (ABattleSimulationCharacter* SimulationCharacter : SimulationCharacterOrder)
	{
		if (!IsValid(SimulationCharacter)) continue;
		if (UCharacterPassiveComponent* PassiveComponent = SimulationCharacter->GetPassiveComponent()) PassiveComponent->NotifyBattleActionStart(WorldBattleAction);
		if (UMuksiStatusEffectComponent* StatusEffectComponent = SimulationCharacter->GetStatusEffectComponent()) StatusEffectComponent->NotifyBattleActionStart(WorldBattleAction);
	}
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
	if (!IndicatorCoords.IsEmpty()) SimulationGridManager->SetExchangeIndicator(Action.Card->AttackType.AttackType, IndicatorCoords, !Action.bPlayerAction);
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
	HandleSimulationActionFinished();
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
	const bool bSimulationCompleted = NextExchangeIndex >= MaxExchangeCount;
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
		SimulationSequenceManager->BattleActionStartDelegate.RemoveAll(this);
		SimulationSequenceManager->Destroy();
		SimulationSequenceManager = nullptr;
	}
	if (SimulationGridManager)
	{
		SimulationGridManager->Destroy();
		SimulationGridManager = nullptr;
	}
	SourceGridManager = nullptr;
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : SimulationCharacterMap)
	{
		if (IsValid(Pair.Value)) Pair.Value->Destroy();
	}
	SimulationCharacterMap.Empty();
	SimulationCharacterOrder.Empty();
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
	StateChangedDelegate.Broadcast(this, SimulationState);
}
