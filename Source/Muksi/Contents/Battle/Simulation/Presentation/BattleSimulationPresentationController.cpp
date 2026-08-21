#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldManager.h"

bool UBattleSimulationPresentationController::Initialize(ABattleSimulationManager* InSimulationManager, ABattleSimulationWorldManager* InADWorldManager, ABattleSimulationWorldManager* InDDWorldManager, ABattleSimulationWorldManager* InDAWorldManager, bool bInEnablePostProcess, TSubclassOf<ABattleSimulationPostProcessVolume> InPostProcessVolumeClass, float InFastForwardTimeScale)
{
	if (!IsValid(InSimulationManager) || !IsValid(InSimulationManager->GetBattleManager()) || !IsValid(InADWorldManager) || !IsValid(InDDWorldManager) || !IsValid(InDAWorldManager)) return false;
	SimulationManager = InSimulationManager;
	ADWorldManager = InADWorldManager;
	DDWorldManager = InDDWorldManager;
	DAWorldManager = InDAWorldManager;
	bEnableSimulationPostProcess = bInEnablePostProcess;
	SimulationPostProcessVolumeClass = InPostProcessVolumeClass;
	FastForwardSimulationTimeScale = FMath::Max(1.0f, InFastForwardTimeScale);
	ApplyPlayerSimulationView();
	return true;
}

void UBattleSimulationPresentationController::Shutdown()
{
	ExitSimulationPresentation(true);
	TimeScaleChangedDelegate.Clear();
	ViewChangedDelegate.Clear();
	AvailabilityChangedDelegate.Clear();
	PresentationCharactersChangedDelegate.Clear();
	ADWorldManager = nullptr;
	DDWorldManager = nullptr;
	DAWorldManager = nullptr;
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
	ViewChangedDelegate.Broadcast(PlayerSimulationView);
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewAvailable(bool bAvailable)
{
	if (!bAvailable) bPlayerSimulationViewChangeLocked = false;
	if (bPlayerSimulationViewAvailable == bAvailable) return;
	bPlayerSimulationViewAvailable = bAvailable;
	ApplyPlayerSimulationView();
	BroadcastPresentationCharacters();
	AvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

void UBattleSimulationPresentationController::SetPlayerSimulationViewChangeLocked(bool bLocked)
{
	if (bPlayerSimulationViewChangeLocked == bLocked) return;
	bPlayerSimulationViewChangeLocked = bLocked;
	AvailabilityChangedDelegate.Broadcast(CanChangePlayerSimulationView());
}

ABattleSimulationWorldManager* UBattleSimulationPresentationController::GetPlayerPresentationWorldManager() const
{
	return PlayerSimulationView == EBattlePlayerSimulationView::DeceivedSelf ? DDWorldManager.Get() : ADWorldManager.Get();
}

ABattleSimulationWorldManager* UBattleSimulationPresentationController::GetPlayerTargetingWorldManager() const
{
	return ADWorldManager.Get();
}

ABattleCharacterBase* UBattleSimulationPresentationController::GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const
{
	if (!IsValid(SourceCharacter)) return nullptr;
	if (!bPlayerSimulationViewAvailable) return const_cast<ABattleCharacterBase*>(SourceCharacter);
	ABattleSimulationWorldManager* WorldManager = GetPlayerPresentationWorldManager();
	ABattleSimulationCharacter* SimulationCharacter = IsValid(WorldManager) ? WorldManager->GetSimulationCharacter(SourceCharacter) : nullptr;
	return IsValid(SimulationCharacter) ? static_cast<ABattleCharacterBase*>(SimulationCharacter) : const_cast<ABattleCharacterBase*>(SourceCharacter);
}

bool UBattleSimulationPresentationController::EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!CreateSimulationPostProcess()) return false;
	HideSourceCharacters(SourceCharacters);
	SetPlayerSimulationViewAvailable(true);
	ViewChangedDelegate.Broadcast(PlayerSimulationView);
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
	if (IsValid(ADWorldManager)) ADWorldManager->ClearRuntimeSimulationPreview();
	if (IsValid(DDWorldManager)) DDWorldManager->ClearRuntimeSimulationPreview();
	if (IsValid(DAWorldManager)) DAWorldManager->ClearRuntimeSimulationPreview();
}

void UBattleSimulationPresentationController::StartSimulationFastForward()
{
	SetSimulationTimeScale(FastForwardSimulationTimeScale);
}

void UBattleSimulationPresentationController::StopSimulationFastForward()
{
	SetSimulationTimeScale(1.0f);
}

void UBattleSimulationPresentationController::ApplyPlayerSimulationView()
{
	ABattleSimulationWorldManager* VisibleWorldManager = bPlayerSimulationViewAvailable ? GetPlayerPresentationWorldManager() : nullptr;
	if (IsValid(ADWorldManager) && ADWorldManager.Get() != VisibleWorldManager) ADWorldManager->SetWorldVisible(false);
	if (IsValid(DDWorldManager) && DDWorldManager.Get() != VisibleWorldManager) DDWorldManager->SetWorldVisible(false);
	if (IsValid(DAWorldManager)) DAWorldManager->SetWorldVisible(false);
	if (IsValid(VisibleWorldManager)) VisibleWorldManager->SetWorldVisible(true);
}

void UBattleSimulationPresentationController::BroadcastPresentationCharacters()
{
	ABattleManager* BattleManager = IsValid(SimulationManager) ? SimulationManager->GetBattleManager() : nullptr;
	if (!IsValid(BattleManager)) return;
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();
	if (!IsValid(RuntimeContext)) return;
	ABattleCharacterBase* PlayerCharacter = GetPresentationCharacter(RuntimeContext->GetPlayerCharacter());
	ABattleCharacterBase* EnemyCharacter = GetPresentationCharacter(RuntimeContext->GetEnemyCharacter());
	if (!IsValid(PlayerCharacter) || !IsValid(EnemyCharacter)) return;
	PresentationCharactersChangedDelegate.Broadcast(PlayerCharacter, EnemyCharacter);
}

bool UBattleSimulationPresentationController::CreateSimulationPostProcess()
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
	UWorld* World = IsValid(SimulationManager) ? SimulationManager->GetWorld() : nullptr;
	if (!World || !SimulationPostProcessVolumeClass) return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SimulationManager;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SimulationPostProcessVolume = World->SpawnActor<ABattleSimulationPostProcessVolume>(SimulationPostProcessVolumeClass, SimulationManager->GetActorTransform(), SpawnParameters);
	if (!SimulationPostProcessVolume) return false;
	SimulationPostProcessVolume->ActivateSimulationPostProcess();
	return true;
}

void UBattleSimulationPresentationController::DestroySimulationPostProcess()
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
	TimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}

void UBattleSimulationPresentationController::RestoreSimulationTimeScale()
{
	if (!bHasCapturedGlobalTimeDilation) return;
	const bool bWasFastForwarding = !FMath::IsNearlyEqual(CurrentSimulationTimeScale, 1.0f);
	UGameplayStatics::SetGlobalTimeDilation(SimulationManager, CapturedGlobalTimeDilation);
	CurrentSimulationTimeScale = 1.0f;
	bHasCapturedGlobalTimeDilation = false;
	CapturedGlobalTimeDilation = 1.0f;
	if (bWasFastForwarding) TimeScaleChangedDelegate.Broadcast(CurrentSimulationTimeScale);
}
