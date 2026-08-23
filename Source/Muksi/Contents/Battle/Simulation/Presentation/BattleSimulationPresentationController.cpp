#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

bool UBattleSimulationPresentationController::Initialize(ABattleSimulationManager* InSimulationManager)
{
	if (!IsValid(InSimulationManager) || !IsValid(InSimulationManager->GetBattleManager())) 
		return false;
	SimulationManager = InSimulationManager;

	TargetingPresentationController = NewObject<UTargetingPresentationController>(this);

	ABattleGridManager* GridManager = InSimulationManager->GetBattleGridManager();

	if (!IsValid(TargetingPresentationController.Get()) || !IsValid(GridManager))
		return false;

	TargetingPresentationController->Initialize(GridManager);

	return true;
}

void UBattleSimulationPresentationController::Shutdown()
{
	ExitSimulationPresentation(true);
	TargetingPresentationController = nullptr;
	SimulationManager = nullptr;
}

bool UBattleSimulationPresentationController::RequestPlayerSimulationView(EBattlePlayerSimulationView NewView)
{
	if (!CanChangePlayerSimulationView()) return false;
	SetPlayerSimulationView(NewView);
	return true;
}

bool UBattleSimulationPresentationController::TogglePlayerSimulationView()
{
	const EBattlePlayerSimulationView NewView = PlayerSimulationView == EBattlePlayerSimulationView::ActualSelf ? EBattlePlayerSimulationView::DeceivedSelf : EBattlePlayerSimulationView::ActualSelf;
	return RequestPlayerSimulationView(NewView);
}

void UBattleSimulationPresentationController::SetPlayerSimulationView(EBattlePlayerSimulationView NewView)
{
	if (PlayerSimulationView == NewView) return;
	PlayerSimulationView = NewView;
	if (bSimulationPresentationActive) SynchronizeSimulationPresentation();
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewChangeLocked(bool bLocked)
{
	if (bPlayerSimulationViewChangeLocked == bLocked) return;
	bPlayerSimulationViewChangeLocked = bLocked;
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
	if (!bSimulationPresentationActive) return const_cast<ABattleCharacterBase*>(SourceCharacter);
	UBattleSimulationWorldRuntime* WorldRuntime = GetPlayerPresentationWorldRuntime();
	ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldRuntime) ? WorldRuntime->GetSimulationCharacter(SourceCharacter) : nullptr;
	return IsValid(SimulationCharacter) ? static_cast<ABattleCharacterBase*>(SimulationCharacter) : const_cast<ABattleCharacterBase*>(SourceCharacter);
}

bool UBattleSimulationPresentationController::EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!CreateSimulationPostProcess()) return false;
	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter) || SourceCharacterHiddenStates.Contains(SourceCharacter)) continue;
		SourceCharacterHiddenStates.Add(SourceCharacter, SourceCharacter->IsHidden());
	}
	CaptureSimulationTimeScaleBaseline();
	PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	bSimulationPresentationActive = true;
	SynchronizeSimulationPresentation();
	return true;
}

void UBattleSimulationPresentationController::ExitSimulationPresentation(bool bClearExecutionPreviews)
{
	if (bClearExecutionPreviews) ExecutionPreviewsByWorld.Empty();
	bSimulationPresentationActive = false;
	bPlayerSimulationViewChangeLocked = false;
	SynchronizeSimulationPresentation();
	SourceCharacterHiddenStates.Empty();
	DestroySimulationPostProcess();
	RestoreSimulationTimeScale();
}

void UBattleSimulationPresentationController::ClearAllExecutionPreviews()
{
	ExecutionPreviewsByWorld.Empty();
	if (bSimulationPresentationActive) ClearDisplayedExecutionPreview();
}

void UBattleSimulationPresentationController::UpdateExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting)
{
	if (!IsValid(WorldRuntime) || !IsValid(Action.Card.Get())) return;
	const EBattleSimulationWorldType WorldType = WorldRuntime->GetWorldType();
	if (WorldType != EBattleSimulationWorldType::PlayerActualEnemyDeceived && WorldType != EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived) return;
	FBattleSimulationExecutionPreview& ExecutionPreview = ExecutionPreviewsByWorld.FindOrAdd(WorldType);
	ExecutionPreview.Action = Action;
	ExecutionPreview.ResolvedTargeting = ResolvedTargeting;
	if (bSimulationPresentationActive && WorldRuntime == GetPlayerPresentationWorldRuntime()) DisplayExecutionPreview(WorldRuntime, ExecutionPreview);
}

void UBattleSimulationPresentationController::RemoveExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime)
{
	if (!IsValid(WorldRuntime)) return;
	ExecutionPreviewsByWorld.Remove(WorldRuntime->GetWorldType());
	if (bSimulationPresentationActive && WorldRuntime == GetPlayerPresentationWorldRuntime()) ClearDisplayedExecutionPreview();
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

void UBattleSimulationPresentationController::SynchronizeSimulationPresentation()
{
	//Set AA Characters Visibility
	if (bSimulationPresentationActive)
	{
		for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
		{
			ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
			if (IsValid(SourceCharacter)) SourceCharacter->SetActorHiddenInGame(true);
		}
	}
	else
	{
		for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
		{
			ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
			if (IsValid(SourceCharacter)) SourceCharacter->SetActorHiddenInGame(Pair.Value);
		}
	}
	//Set Simulation Characters Visibility
	ABattleSimulationManager* Manager = SimulationManager.Get();
	if (!IsValid(Manager))
	{
		ClearDisplayedExecutionPreview();
		return;
	}
	UBattleSimulationWorldRuntime* VisibleWorldRuntime = bSimulationPresentationActive ? GetPlayerPresentationWorldRuntime() : nullptr;
	const EBattleSimulationWorldType WorldTypes[] = { EBattleSimulationWorldType::PlayerActualEnemyDeceived, EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived, EBattleSimulationWorldType::PlayerDeceivedEnemyActual };
	for (EBattleSimulationWorldType WorldType : WorldTypes)
	{
		UBattleSimulationWorldRuntime* WorldRuntime = Manager->GetSimulationWorldRuntime(WorldType);
		if (IsValid(WorldRuntime)) WorldRuntime->SetCharactersVisible(WorldRuntime == VisibleWorldRuntime);
	}
	if (!IsValid(VisibleWorldRuntime))
	{
		ClearDisplayedExecutionPreview();
		return;
	}

	//Display ExecutionPreview
	const FBattleSimulationExecutionPreview* ExecutionPreview = ExecutionPreviewsByWorld.Find(VisibleWorldRuntime->GetWorldType());
	if (ExecutionPreview) 
		DisplayExecutionPreview(VisibleWorldRuntime, *ExecutionPreview);
	else ClearDisplayedExecutionPreview();
}

void UBattleSimulationPresentationController::DisplayExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationExecutionPreview& ExecutionPreview)
{
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(GridManager)) return;
	ClearDisplayedExecutionPreview();
	ABattleCharacterBase* RuntimeAttacker = ExecutionPreview.Action.Attacker.Get();
	TArray<FHexOffsetCoord> IndicatorCoords;
	const int32 StepCount = ExecutionPreview.Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = ExecutionPreview.Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData) continue;
		FResolvedTargeting StepResolvedTargeting;
		if (!ResolveSimulationStepTargeting(WorldRuntime, ExecutionPreview, StepIndex, StepResolvedTargeting)) continue;
		AppendStepPreview(WorldRuntime, RuntimeAttacker, ExecutionPreview.Action, StepIndex, *StepData, StepResolvedTargeting, IndicatorCoords);
	}
	if (!IndicatorCoords.IsEmpty()) GridManager->SetExchangeIndicator(ExecutionPreview.Action.Card->CardTypeInfo, IndicatorCoords, !ExecutionPreview.Action.bPlayerAction);
}

void UBattleSimulationPresentationController::ClearDisplayedExecutionPreview()
{
	if (TargetingPresentationController) TargetingPresentationController->ClearExecutionPreview();
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(GridManager)) return;
	GridManager->AllClearGridHovered();
	GridManager->AllClearExchangeIndicator();
}

void UBattleSimulationPresentationController::AppendStepPreview(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FResolvedTargeting& StepResolvedTargeting, TArray<FHexOffsetCoord>& OutIndicatorCoords)
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
	if (bShowPreview) TargetingPresentationController->AddResolvedStepPreview(RuntimeAttacker, WorldRuntime->GetWorldType(), Action.Card->TargetingData, StepResolvedTargeting, StepIndex, Settings, !Action.bPlayerAction);
}

ABattleGridManager* UBattleSimulationPresentationController::GetGridManager() const
{
	return IsValid(SimulationManager.Get()) ? SimulationManager->GetBattleGridManager() : nullptr;
}

bool UBattleSimulationPresentationController::ResolveSimulationStepTargeting(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationExecutionPreview& ExecutionPreview, int32 StepIndex, FResolvedTargeting& OutResolvedTargeting) const
{
	if (StepIndex == ExecutionPreview.Action.Card->TargetingData.Steps.Num() - 1)
	{
		OutResolvedTargeting = ExecutionPreview.ResolvedTargeting;
		return true;
	}
	ABattleGridManager* GridManager = GetGridManager();
	if (!IsValid(GridManager)) return false;
	FBattleAction RuntimeAction = ExecutionPreview.Action;
	return FBattleTargetResolver::ResolveActionThroughStep(RuntimeAction, GridManager, WorldRuntime->GetWorldType(), StepIndex, OutResolvedTargeting);
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
