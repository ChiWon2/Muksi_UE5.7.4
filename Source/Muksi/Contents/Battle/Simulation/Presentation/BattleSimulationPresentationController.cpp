#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

bool UBattleSimulationPresentationController::Initialize(ABattleSimulationManager* InSimulationManager)
{
	if (!IsValid(InSimulationManager) || !IsValid(InSimulationManager->GetBattleManager())) return false;
	SimulationManager = InSimulationManager;
	TargetingPresentationController = NewObject<UTargetingPresentationController>(this);
	ABattleGridManager* GridManager = InSimulationManager->GetBattleGridManager();
	if (!IsValid(TargetingPresentationController.Get()) || !IsValid(GridManager)) return false;
	TargetingPresentationController->Initialize(GridManager);
	ApplyPlayerSimulationView();
	return true;
}

void UBattleSimulationPresentationController::Shutdown()
{
	ExitSimulationPresentation(true);
	PresentationStates.Empty();
	TargetingPresentationController = nullptr;
	SimulationManager = nullptr;
}

bool UBattleSimulationPresentationController::SetPlayerSimulationView(EBattlePlayerSimulationView NewView)
{
	if (!CanChangePlayerSimulationView()) return false;
	SetPlayerSimulationViewInternal(NewView);
	return true;
}

bool UBattleSimulationPresentationController::TogglePlayerSimulationView()
{
	const EBattlePlayerSimulationView NewView = PlayerSimulationView == EBattlePlayerSimulationView::ActualSelf ? EBattlePlayerSimulationView::DeceivedSelf : EBattlePlayerSimulationView::ActualSelf;
	return SetPlayerSimulationView(NewView);
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewInternal(EBattlePlayerSimulationView NewView)
{
	if (PlayerSimulationView == NewView) return;
	PlayerSimulationView = NewView;
	ApplyPlayerSimulationView();
	BroadcastPresentationCharacters();
	if (IsValid(SimulationManager.Get())) SimulationManager->PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewAvailable(bool bAvailable)
{
	if (!bAvailable) bPlayerSimulationViewChangeLocked = false;
	if (bPlayerSimulationViewAvailable == bAvailable) return;
	bPlayerSimulationViewAvailable = bAvailable;
	ApplyPlayerSimulationView();
	BroadcastPresentationCharacters();
	if (IsValid(SimulationManager.Get())) SimulationManager->PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewChangeLocked(bool bLocked)
{
	if (bPlayerSimulationViewChangeLocked == bLocked) return;
	bPlayerSimulationViewChangeLocked = bLocked;
	if (IsValid(SimulationManager.Get())) SimulationManager->PlayerSimulationViewAvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

UBattleSimulationWorldRuntime* UBattleSimulationPresentationController::GetPlayerPresentationWorldRuntime() const
{
	if (!IsValid(SimulationManager.Get())) return nullptr;
	const EBattleSimulationWorldType WorldType = PlayerSimulationView == EBattlePlayerSimulationView::DeceivedSelf ? EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived : EBattleSimulationWorldType::PlayerActualEnemyDeceived;
	return SimulationManager->GetSimulationWorldRuntime(WorldType);
}

ABattleCharacterBase* UBattleSimulationPresentationController::GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter)) return nullptr;
	if (!bPlayerSimulationViewAvailable) return const_cast<ABattleCharacterBase*>(SourceCharacter);
	UBattleSimulationWorldRuntime* WorldRuntime = GetPlayerPresentationWorldRuntime();
	ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldRuntime) ? WorldRuntime->GetSimulationCharacter(SourceCharacter) : nullptr;
	return IsValid(SimulationCharacter) ? static_cast<ABattleCharacterBase*>(SimulationCharacter) : const_cast<ABattleCharacterBase*>(SourceCharacter);
}

bool UBattleSimulationPresentationController::EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!CreateSimulationPostProcess()) return false;
	HideSourceCharacters(SourceCharacters);
	SetPlayerSimulationViewAvailable(true);
	if (IsValid(SimulationManager.Get())) SimulationManager->PlayerSimulationViewChangedDelegate.Broadcast(PlayerSimulationView);
	CaptureSimulationTimeScaleBaseline();
	SetSimulationTimeScale(1.0f);
	return true;
}

void UBattleSimulationPresentationController::ExitSimulationPresentation(bool bClearRuntimePreview)
{
	SetPlayerSimulationViewAvailable(false);
	if (bClearRuntimePreview) ClearRuntimeSimulationPreview();
	RestoreSourceCharacters();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
}

void UBattleSimulationPresentationController::ClearRuntimeSimulationPreview()
{
	PresentationStates.Empty();
	ClearDisplayedExecutionPresentation();
}

void UBattleSimulationPresentationController::PresentSimulationExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting)
{
	if (!IsValid(WorldRuntime)) return;
	FBattleSimulationPresentationState& PresentationState = PresentationStates.FindOrAdd(WorldRuntime->GetWorldType());
	PresentationState.Action = Action;
	PresentationState.ResolvedTargeting = ResolvedTargeting;
	if (bPlayerSimulationViewAvailable && WorldRuntime == GetPlayerPresentationWorldRuntime()) RefreshSimulationExecutionPresentation(WorldRuntime, PresentationState);
}

void UBattleSimulationPresentationController::ClearSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime)
{
	if (!IsValid(WorldRuntime)) return;
	PresentationStates.Remove(WorldRuntime->GetWorldType());
	if (WorldRuntime != GetPlayerPresentationWorldRuntime()) return;
	ClearDisplayedExecutionPresentation();
}

void UBattleSimulationPresentationController::StartSimulationFastForward()
{
	const float TimeScale = IsValid(SimulationManager.Get()) ? SimulationManager->GetFastForwardSimulationTimeScale() : 1.0f;
	SetSimulationTimeScale(TimeScale);
}

void UBattleSimulationPresentationController::StopSimulationFastForward()
{
	SetSimulationTimeScale(1.0f);
}

void UBattleSimulationPresentationController::ApplyPlayerSimulationView()
{
	UBattleSimulationWorldRuntime* VisibleWorldRuntime = bPlayerSimulationViewAvailable ? GetPlayerPresentationWorldRuntime() : nullptr;
	if (IsValid(SimulationManager.Get()))
	{
		const EBattleSimulationWorldType WorldTypes[] = { EBattleSimulationWorldType::PlayerActualEnemyDeceived, EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived, EBattleSimulationWorldType::PlayerDeceivedEnemyActual };
		for (EBattleSimulationWorldType WorldType : WorldTypes)
		{
			UBattleSimulationWorldRuntime* WorldRuntime = SimulationManager->GetSimulationWorldRuntime(WorldType);
			if (IsValid(WorldRuntime)) WorldRuntime->SetCharactersVisible(WorldRuntime == VisibleWorldRuntime);
		}
	}
	if (!IsValid(VisibleWorldRuntime))
	{
		ClearDisplayedExecutionPresentation();
		return;
	}
	const FBattleSimulationPresentationState* PresentationState = PresentationStates.Find(VisibleWorldRuntime->GetWorldType());
	if (PresentationState) RefreshSimulationExecutionPresentation(VisibleWorldRuntime, *PresentationState);
	else ClearDisplayedExecutionPresentation();
}

void UBattleSimulationPresentationController::RefreshSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPresentationState& PresentationState)
{
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(WorldRuntime) || !IsValid(GridManager) || !IsValid(PresentationState.Action.Card.Get()) || !TargetingPresentationController) return;
	ClearDisplayedExecutionPresentation();
	ABattleCharacterBase* RuntimeAttacker = ResolveRuntimeAttacker(WorldRuntime, PresentationState.Action);
	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = PresentationState.Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = PresentationState.Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData) continue;
		FResolvedTargeting StepResolvedTargeting;
		if (!ResolveSimulationStepTargeting(WorldRuntime, PresentationState, StepIndex, StepResolvedTargeting)) continue;
		PresentSimulationStep(WorldRuntime, RuntimeAttacker, PresentationState.Action, StepIndex, *StepData, StepResolvedTargeting, IndicatorCoords);
	}
	if (!IndicatorCoords.IsEmpty()) GridManager->SetExchangeIndicator(PresentationState.Action.Card->CardTypeInfo, IndicatorCoords, !PresentationState.Action.bPlayerAction);
}

void UBattleSimulationPresentationController::ClearDisplayedExecutionPresentation()
{
	if (TargetingPresentationController) TargetingPresentationController->ClearExecutionPreview();
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(GridManager)) return;
	GridManager->AllClearGridHovered();
	GridManager->AllClearExchangeIndicator();
}

void UBattleSimulationPresentationController::PresentSimulationStep(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FResolvedTargeting& StepResolvedTargeting, TArray<FHexOffsetCoord>& OutIndicatorCoords)
{
	const FTargetingPhasePresentationSettings& Settings = StepData.AdvancedSettings.Presentation.SimulationPhase;
	if (Settings.bShowIndicator)
	{
		TArray<FHexOffsetCoord> StepIndicatorCoords = StepResolvedTargeting.AffectedCoords;
		if (StepIndicatorCoords.IsEmpty())
		{
			const FTargetingStepResult* StepResult = StepResolvedTargeting.GetStep(StepIndex);
			if (StepResult && StepResult->HasSelectedCoord()) StepIndicatorCoords.Add(StepResult->SelectedCoord);
		}
		for (const FHexOffsetCoord& Coord : StepIndicatorCoords) OutIndicatorCoords.AddUnique(Coord);
	}
	const bool bShowPreview = Settings.bShowSelectionPreview || Settings.bShowPathPreview || Settings.bShowAreaPreview;
	if (bShowPreview && TargetingPresentationController) TargetingPresentationController->AddResolvedStepPreview(RuntimeAttacker, WorldRuntime->GetWorldType(), Action.Card->TargetingData, StepResolvedTargeting, StepIndex, Settings, !Action.bPlayerAction);
}

ABattleGridManager* UBattleSimulationPresentationController::GetGridManager() const
{
	return IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleGridManager() : nullptr;
}

ABattleCharacterBase* UBattleSimulationPresentationController::ResolveRuntimeAttacker(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action) const
{
	ABattleCharacterBase* RuntimeAttacker = Action.Attacker.Get();
	ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldRuntime) ? WorldRuntime->GetSimulationCharacter(RuntimeAttacker) : nullptr;
	return IsValid(SimulationCharacter) ? SimulationCharacter : RuntimeAttacker;
}

bool UBattleSimulationPresentationController::ResolveSimulationStepTargeting(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPresentationState& PresentationState, int32 StepIndex, FResolvedTargeting& OutResolvedTargeting) const
{
	if (!IsValid(WorldRuntime) || !IsValid(PresentationState.Action.Card.Get())) return false;
	if (StepIndex == PresentationState.Action.Card->TargetingData.Steps.Num() - 1)
	{
		OutResolvedTargeting = PresentationState.ResolvedTargeting;
		return true;
	}
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(GridManager)) return false;
	FBattleAction RuntimeAction = PresentationState.Action;
	RuntimeAction.Attacker = ResolveRuntimeAttacker(WorldRuntime, PresentationState.Action);
	return FBattleTargetResolver::ResolveActionThroughStep(RuntimeAction, GridManager, WorldRuntime->GetWorldType(), StepIndex, OutResolvedTargeting);
}

void UBattleSimulationPresentationController::BroadcastPresentationCharacters()
{
	ABattleManager* BattleManager = IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleManager() : nullptr;
	if (!IsValid(BattleManager)) return;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	if (!IsValid(RuntimeContext)) return;
	ABattleCharacterBase* PlayerCharacter = GetPresentationCharacter(RuntimeContext->GetPlayerCharacter());
	ABattleCharacterBase* EnemyCharacter = GetPresentationCharacter(RuntimeContext->GetEnemyCharacter());
	if (!IsValid(PlayerCharacter) || !IsValid(EnemyCharacter)) return;
	SimulationManager->PresentationCharactersChangedDelegate.Broadcast(PlayerCharacter, EnemyCharacter);
}

bool UBattleSimulationPresentationController::CreateSimulationPostProcess()
{
	if (!IsValid(SimulationManager.Get()) || !SimulationManager->IsSimulationPostProcessEnabled())
	{
		DestroySimulationPostProcess();
		return true;
	}
	if (IsValid(SimulationPostProcessVolume.Get()))
	{
		SimulationPostProcessVolume->ActivateSimulationPostProcess();
		return true;
	}
	UWorld* World = IsValid(SimulationManager.Get()) ? SimulationManager->GetWorld() : nullptr;
	TSubclassOf<ABattleSimulationPostProcessVolume> PostProcessVolumeClass = SimulationManager->GetSimulationPostProcessVolumeClass();
	if (!World || !PostProcessVolumeClass) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SimulationManager;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SimulationPostProcessVolume = World->SpawnActor<ABattleSimulationPostProcessVolume>(PostProcessVolumeClass, SimulationManager->GetActorTransform(), SpawnParameters);
	if (!SimulationPostProcessVolume) return false;
	SimulationPostProcessVolume->ActivateSimulationPostProcess();
	return true;
}

void UBattleSimulationPresentationController::DestroySimulationPostProcess()
{
	if (!IsValid(SimulationPostProcessVolume.Get()))
	{
		SimulationPostProcessVolume = nullptr;
		return;
	}
	SimulationPostProcessVolume->DeactivateSimulationPostProcess();
	SimulationPostProcessVolume->Destroy();
	SimulationPostProcessVolume = nullptr;
}

void UBattleSimulationPresentationController::HideSourceCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter)) continue;
		if (!SourceCharacterHiddenStates.Contains(SourceCharacter)) SourceCharacterHiddenStates.Add(SourceCharacter, SourceCharacter->IsHidden());
		SourceCharacter->SetActorHiddenInGame(true);
	}
}

void UBattleSimulationPresentationController::RestoreSourceCharacters()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
		if (IsValid(SourceCharacter)) SourceCharacter->SetActorHiddenInGame(Pair.Value);
	}
	SourceCharacterHiddenStates.Empty();
}

void UBattleSimulationPresentationController::CaptureSimulationTimeScaleBaseline()
{
	if (bHasCapturedGlobalTimeDilation) return;
	CapturedGlobalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(SimulationManager);
	bHasCapturedGlobalTimeDilation = true;
	CurrentSimulationTimeScale = 1.0f;
}

void UBattleSimulationPresentationController::SetSimulationTimeScale(float NewTimeScale)
{
	const float SafeTimeScale = FMath::Max(NewTimeScale, 0.01f);
	if (!bHasCapturedGlobalTimeDilation) CaptureSimulationTimeScaleBaseline();
	if (FMath::IsNearlyEqual(CurrentSimulationTimeScale, SafeTimeScale)) return;
	CurrentSimulationTimeScale = SafeTimeScale;
	UGameplayStatics::SetGlobalTimeDilation(SimulationManager, CapturedGlobalTimeDilation * CurrentSimulationTimeScale);
	SimulationManager->SimulationTimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}

void UBattleSimulationPresentationController::RestoreSimulationTimeScale()
{
	if (!bHasCapturedGlobalTimeDilation) return;
	const bool bWasFastForwarding = !FMath::IsNearlyEqual(CurrentSimulationTimeScale, 1.0f);
	UGameplayStatics::SetGlobalTimeDilation(SimulationManager, CapturedGlobalTimeDilation);
	CurrentSimulationTimeScale = 1.0f;
	bHasCapturedGlobalTimeDilation = false;
	CapturedGlobalTimeDilation = 1.0f;
	if (bWasFastForwarding && IsValid(SimulationManager.Get())) SimulationManager->SimulationTimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}
