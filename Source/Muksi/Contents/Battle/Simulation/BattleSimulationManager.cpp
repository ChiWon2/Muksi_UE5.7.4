#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"
#include "Muksi/Contents/MuksiWorldManagerSubsystem.h"

// ============================================================================
// 생명주기 / 상태 조회
// ============================================================================
ABattleSimulationManager::ABattleSimulationManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SimulationPostProcessVolumeClass = ABattleSimulationPostProcessVolume::StaticClass();
}

void ABattleSimulationManager::BeginPlay()
{
	Super::BeginPlay();

	SimulationTargetingPresentationController = NewObject<UTargetingPresentationController>(this);

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->RegisterManager<ABattleSimulationManager>(this);
	}

	if (!TryBindBattleFlow())
	{
		BattleFlowBindingTimerHandle = GetWorldTimerManager().SetTimerForNextTick(
			FTimerDelegate::CreateUObject(this, &ABattleSimulationManager::BindBattleFlowDeferred));
	}
}

void ABattleSimulationManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(BattleFlowBindingTimerHandle);

	if (BattleManager)
	{
		BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
		BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	}

	ClearRuntimeSimulationPreview();
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	DestroySimulationRuntime();

	BattleGridManager = nullptr;
	BattleRuntimeContext = nullptr;
	BattleManager = nullptr;

	if (UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this))
	{
		ManagerSubsystem->UnregisterManager<ABattleSimulationManager>(this);
	}

	Super::EndPlay(EndPlayReason);
}

bool ABattleSimulationManager::IsSimulationRunning() const
{
	return SimulationState != EBattleSimulationState::Idle && SimulationState != EBattleSimulationState::Completed;
}

bool ABattleSimulationManager::TryBindBattleFlow()
{
	UMuksiWorldManagerSubsystem* ManagerSubsystem = UMuksiWorldManagerSubsystem::Get(this);
	if (!ManagerSubsystem)
	{
		return false;
	}

	ABattleManager* FoundBattleManager = ManagerSubsystem->GetManager<ABattleManager>();
	if (!IsValid(FoundBattleManager) || !IsValid(FoundBattleManager->GetBattleRuntimeContext()))
	{
		return false;
	}

	if (BattleManager && BattleManager != FoundBattleManager)
	{
		BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
		BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	}

	BattleManager = FoundBattleManager;
	BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();
	BattleGridManager = ManagerSubsystem->GetManager<ABattleGridManager>();

	if (!IsValid(BattleGridManager))
	{
		return false;
	}

	MaxExchangeCount = BattleManager->GetMaxExchangeCount();
	if (SimulationTargetingPresentationController)
	{
		SimulationTargetingPresentationController->Initialize(BattleGridManager);
	}

	BattleManager->ChangePhaseDelegate.RemoveDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
	BattleManager->ChangePhaseDelegate.AddUniqueDynamic(this, &ABattleSimulationManager::HandleBattlePhaseChanged);
	BattleManager->PhaseUIFinishedDelegate.RemoveAll(this);
	BattleManager->PhaseUIFinishedDelegate.AddUObject(this, &ABattleSimulationManager::HandleBattlePhaseUIFinished);
	return true;
}

void ABattleSimulationManager::BindBattleFlowDeferred()
{
	if (!TryBindBattleFlow())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to bind battle flow."));
	}
}

void ABattleSimulationManager::HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	(void)OldPhase;

	switch (NewPhase)
	{
	case EBattlePhase::CardReveal:
		if (!PrepareCurrentExchangeSimulation())
		{
			UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to prepare exchange simulation."));
			GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::CardReveal)
				{
					BattleManager->RestartCurrentExchangeCardSelection();
				}
			}));
		}
		break;

	case EBattlePhase::SimulationSequence:
		if (!StartCurrentExchangeSimulation())
		{
			UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to execute current exchange simulation."));
		}
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

	if (NewPhase != EBattlePhase::RoundStart || IsSimulationRunning())
	{
		return;
	}

	if (!InitializeRoundSimulation())
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] Failed to initialize round simulation."));
		return;
	}

	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]()
	{
		if (BattleManager && BattleManager->GetCurrentPhase() == EBattlePhase::RoundStart)
		{
			BattleManager->NotifyPhaseExecutionFinished();
		}
	}));
}

bool ABattleSimulationManager::InitializeRoundSimulation()
{
	if (!IsValid(BattleRuntimeContext) || !IsValid(BattleGridManager))
	{
		return false;
	}

	TArray<ABattleCharacterBase*> SourceCharacters;
	SourceCharacters.Add(BattleRuntimeContext->GetPlayerCharacter());
	SourceCharacters.Add(BattleRuntimeContext->GetEnemyCharacter());
	return StartSimulation(BattleGridManager, SourceCharacters);
}

bool ABattleSimulationManager::PrepareCurrentExchangeSimulation()
{
	if (!IsSimulationRunning() || !IsValid(BattleRuntimeContext))
	{
		return false;
	}

	const int32 ExchangeIndex = BattleRuntimeContext->GetCurrentExchange();
	const FBattleAction* PlayerAction = BattleRuntimeContext->GetPlayerExchangeAction(ExchangeIndex);
	const FBattleAction* EnemyAction = BattleRuntimeContext->GetEnemyExchangeAction(ExchangeIndex);
	if (!PlayerAction || !EnemyAction)
	{
		return false;
	}

	CurrentExchange.Reset(ExchangeIndex);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);

	if (!SetEnemyAction(*EnemyAction) || !SetPlayerAction(*PlayerAction))
	{
		CurrentExchange.Reset(ExchangeIndex);
		SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
		return false;
	}

	return true;
}

bool ABattleSimulationManager::StartCurrentExchangeSimulation()
{
	return BattleManager
		&& BattleManager->GetCurrentPhase() == EBattlePhase::SimulationSequence
		&& ExecuteCurrentExchange();
}

void ABattleSimulationManager::NotifySimulationPhaseFinished(int32 FinishedExchangeIndex)
{
	if (!BattleManager || !BattleRuntimeContext
		|| BattleManager->GetCurrentPhase() != EBattlePhase::SimulationSequence
		|| BattleRuntimeContext->GetCurrentExchange() != FinishedExchangeIndex)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSimulationManager] Ignored stale simulation completion. Finished=%d"), FinishedExchangeIndex);
		return;
	}

	BattleManager->NotifyPhaseExecutionFinished();
}

// ============================================================================
// Simulation 시작 및 Exchange 입력
// RoundStart UI 완료 -> StartSimulation / CardReveal -> SetPlayer/EnemyAction
// ============================================================================
bool ABattleSimulationManager::StartSimulation(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!IsValid(BattleRuntimeContext))
	{
		UE_LOG(LogTemp, Error, TEXT("[BattleSimulationManager] BattleRuntimeContext is invalid."));
		return false;
	}

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
	ClearRuntimeSimulationPreview();
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
		BattleRuntimeContext->SetRuntimeCharacterOverride(SourceCharacter, SimulationCharacter);
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

	HandleSimulationActionStarted(ActionPlan.SequenceAction);
	if (!SimulationSequenceManager->StartSequenceWithRequest(Request))
	{
		HandleSimulationActionFinished();
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

void ABattleSimulationManager::HandleSimulationActionStarted(const FBattleAction& Action)
{
	(void)Action;
	ClearRuntimeSimulationPreview();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
}

void ABattleSimulationManager::HandleSimulationActionFinished()
{
	ClearRuntimeSimulationPreview();
	if (BattleGridManager)
	{
		BattleGridManager->AllClearGridHovered();
		BattleGridManager->AllClearExchangeIndicator();
	}
}

void ABattleSimulationManager::RefreshSimulationTargetingPresentation(
	const FBattleAction& Action,
	const FResolvedTargeting& ExecutionResolvedTargeting)
{
	ClearRuntimeSimulationPreview();

	if (!BattleGridManager || !IsValid(Action.Card))
	{
		return;
	}

	BattleGridManager->AllClearGridHovered();
	BattleGridManager->AllClearExchangeIndicator();

	ABattleCharacterBase* RuntimeAttacker = Action.Attacker;
	if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(Action.Attacker))
	{
		RuntimeAttacker = SimulationCharacter;
	}

	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData)
		{
			continue;
		}

		FResolvedTargeting StepResolvedTargeting;
		if (StepIndex == StepCount - 1)
		{
			StepResolvedTargeting = ExecutionResolvedTargeting;
		}
		else if (!ResolveSimulationActionTargetingThroughStep(Action, StepIndex, StepResolvedTargeting))
		{
			UE_LOG(LogTemp, Warning, TEXT("[RuntimeTargetingPresentation] Simulation resolve failed. Step=%d"), StepIndex);
			continue;
		}

		const FTargetingPhasePresentationSettings& PresentationSettings =
			StepData->AdvancedSettings.Presentation.SimulationPhase;

		if (PresentationSettings.bShowIndicator)
		{
			TArray<FHexOffsetCoord> StepIndicatorCoords = StepResolvedTargeting.AffectedCoords;
			if (StepIndicatorCoords.IsEmpty())
			{
				if (const FTargetingStepResult* StepResult = StepResolvedTargeting.GetStep(StepIndex))
				{
					if (StepResult->HasSelectedCoord())
					{
						StepIndicatorCoords.Add(StepResult->SelectedCoord);
					}
				}
			}

			for (const FHexOffsetCoord& Coord : StepIndicatorCoords)
			{
				IndicatorCoords.AddUnique(Coord);
			}
		}

		const bool bShowAnyPreview = PresentationSettings.bShowSelectionPreview
			|| PresentationSettings.bShowPathPreview
			|| PresentationSettings.bShowAreaPreview;
		if (bShowAnyPreview && SimulationTargetingPresentationController)
		{
			SimulationTargetingPresentationController->AddResolvedStepPreview(
				RuntimeAttacker,
				Action.Card->TargetingData,
				StepResolvedTargeting,
				StepIndex,
				PresentationSettings,
				!Action.bPlayerAction);
		}
	}

	if (!IndicatorCoords.IsEmpty())
	{
		BattleGridManager->SetExchangeIndicator(
			Action.Card->AttackType.AttackType,
			IndicatorCoords,
			!Action.bPlayerAction);
	}
}

bool ABattleSimulationManager::ResolveSimulationActionTargetingThroughStep(
	const FBattleAction& Action,
	int32 LastStepIndex,
	FResolvedTargeting& OutResolvedTargeting) const
{
	FBattleAction RuntimeAction = Action;
	if (ABattleSimulationCharacter* SimulationCharacter = GetSimulationCharacter(Action.Attacker))
	{
		RuntimeAction.Attacker = SimulationCharacter;
	}

	return FBattleTargetResolver::ResolveActionThroughStep(
		RuntimeAction,
		BattleGridManager,
		LastStepIndex,
		OutResolvedTargeting);
}

void ABattleSimulationManager::ClearRuntimeSimulationPreview()
{
	if (SimulationTargetingPresentationController)
	{
		SimulationTargetingPresentationController->ClearExecutionPreview();
	}
}

void ABattleSimulationManager::HandleSimulationExecutionStarted(
	const FBattleAction& Action,
	const FBattleExecutionEntry& Entry,
	int32 EntryIndex,
	const FResolvedTargeting& ResolvedTargeting)
{
	(void)Entry;
	(void)EntryIndex;
	RefreshSimulationTargetingPresentation(Action, ResolvedTargeting);
}

void ABattleSimulationManager::HandleSimulationSequenceFinished()
{
	HandleSimulationActionFinished();

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

	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->AppendBattleActionSequenceAction(CurrentExchange.GetFirstAction().SequenceAction);
		BattleRuntimeContext->AppendBattleActionSequenceAction(CurrentExchange.GetSecondAction().SequenceAction);
	}

	if (NextExchangeIndex >= MaxExchangeCount)
	{
		SetSimulationState(EBattleSimulationState::FinishingSimulation);
		RestoreSourceCharacters();
		DestroySimulationPostProcess();
		DestroySimulationRuntime();
		CurrentExchange.Reset(INDEX_NONE);
		SetSimulationState(EBattleSimulationState::Completed);
		NotifySimulationPhaseFinished(FinishedExchangeIndex);
		return;
	}

	CurrentExchange.Reset(NextExchangeIndex);
	SetSimulationState(EBattleSimulationState::WaitingForPlayerTargeting);
	NotifySimulationPhaseFinished(FinishedExchangeIndex);
}

// ============================================================================
// Runtime 정리 / 상태 전환
// ============================================================================
void ABattleSimulationManager::DestroySimulationRuntime()
{
	ClearRuntimeSimulationPreview();

	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->ClearRuntimeCharacterOverrides();
	}

	if (SimulationSequenceManager)
	{
		SimulationSequenceManager->OnSequenceFinished.RemoveAll(this);
		SimulationSequenceManager->OnExecutionEntryStarted.RemoveAll(this);
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
	if (BattleRuntimeContext)
	{
		BattleRuntimeContext->ClearBattleActionSequenceQueue();
	}
	SetSimulationState(EBattleSimulationState::Idle);
}

void ABattleSimulationManager::SetSimulationState(EBattleSimulationState NewState)
{
	SimulationState = NewState;
}
