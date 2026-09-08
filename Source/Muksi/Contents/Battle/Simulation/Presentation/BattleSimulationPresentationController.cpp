#include "Muksi/Contents/Battle/Simulation/Presentation/BattleSimulationPresentationController.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Simulation/BattleSimulationManager.h"
#include "Muksi/Contents/Battle/Simulation/Character/BattleSimulationCharacter.h"
#include "Muksi/Contents/Battle/Simulation/PostProcess/BattleSimulationPostProcessVolume.h"
#include "Muksi/Contents/Battle/Simulation/World/BattleSimulationWorldRuntime.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Presentation/TargetingPresentationController.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Context/TargetingPreviewContext.h"

bool UBattleSimulationPresentationController::Initialize(ABattleSimulationManager* InSimulationManager, UTargetingPresentationController* InTargetingPresentationController)
{
	if (!IsValid(InSimulationManager) || !IsValid(InTargetingPresentationController))
		return false;
	SimulationManager = InSimulationManager;
	TargetingPresentationController = InTargetingPresentationController;
	return true;
}

void UBattleSimulationPresentationController::Shutdown()
{
	ExitSimulationPresentation(true);
	TargetingPresentationController = nullptr;
	SimulationManager = nullptr;
}

float UBattleSimulationPresentationController::GetSimulationTimeScale() const
{
	return IsValid(SimulationManager.Get()) ? SimulationManager->GetCurrentSimulationTimeScale() : 1.0f;
}

UBattleSimulationWorldRuntime* UBattleSimulationPresentationController::GetPrimaryPresentationWorldRuntime() const
{
	return IsValid(SimulationManager.Get()) ? SimulationManager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerActualEnemyDeceived) : nullptr;
}

bool UBattleSimulationPresentationController::EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	if (!CreateSimulationPostProcess())
		return false;

	if (!CreateDeceivedGhosts(SourceCharacters))
	{
		DestroySimulationPostProcess();
		return false;
	}

	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter) || SourceCharacterHiddenStates.Contains(SourceCharacter))
			continue;

		SourceCharacterHiddenStates.Add(SourceCharacter, SourceCharacter->IsHidden());
	}

	bSimulationPresentationActive = true;
	SynchronizeSimulationPresentation();
	return true;
}

void UBattleSimulationPresentationController::ExitSimulationPresentation(bool bClearPreviewData)
{
	if (bClearPreviewData)
		PreviewDataByWorld.Empty();

	bSimulationPresentationActive = false;
	SynchronizeSimulationPresentation();
	DestroyDeceivedGhosts();
	SourceCharacterHiddenStates.Empty();
	DestroySimulationPostProcess();
}

void UBattleSimulationPresentationController::ClearAllPreviewData()
{
	PreviewDataByWorld.Empty();
}

void UBattleSimulationPresentationController::UpdatePreviewData(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult)
{
	if (!IsValid(WorldRuntime) || !IsValid(Action.Card.Get()))
		return;
	const EBattleSimulationWorldType WorldType = WorldRuntime->GetWorldType();
	if (WorldType != EBattleSimulationWorldType::PlayerActualEnemyDeceived)
		return;
	FBattleSimulationPreviewData& PreviewData = PreviewDataByWorld.FindOrAdd(WorldType);
	PreviewData.Action = Action;
	PreviewData.TargetingResult = TargetingResult;
	if (bSimulationPresentationActive && WorldRuntime == GetPrimaryPresentationWorldRuntime())
		DisplayExecutionPreview(WorldRuntime, PreviewData);
}

void UBattleSimulationPresentationController::RemovePreviewData(UBattleSimulationWorldRuntime* WorldRuntime)
{
	if (!IsValid(WorldRuntime))
		return;
	PreviewDataByWorld.Remove(WorldRuntime->GetWorldType());
	if (bSimulationPresentationActive && WorldRuntime == GetPrimaryPresentationWorldRuntime())
		ClearExecutionPreview();
}

void UBattleSimulationPresentationController::SynchronizeSimulationPresentation()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, bool>& Pair : SourceCharacterHiddenStates)
	{
		ABattleCharacterBase* SourceCharacter = Pair.Key.Get();
		if (IsValid(SourceCharacter))
			SourceCharacter->SetActorHiddenInGame(bSimulationPresentationActive ? true : Pair.Value);
	}

	ABattleSimulationManager* Manager = SimulationManager.Get();
	if (!IsValid(Manager))
	{
		ClearExecutionPreview();
		return;
	}

	UBattleSimulationWorldRuntime* ADWorldRuntime = Manager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerActualEnemyDeceived);
	UBattleSimulationWorldRuntime* DDWorldRuntime = Manager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived);
	UBattleSimulationWorldRuntime* DAWorldRuntime = Manager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerDeceivedEnemyActual);

	if (IsValid(ADWorldRuntime))
		ADWorldRuntime->SetCharactersVisible(bSimulationPresentationActive);

	if (IsValid(DDWorldRuntime))
		DDWorldRuntime->SetCharactersVisible(false);

	if (IsValid(DAWorldRuntime))
		DAWorldRuntime->SetCharactersVisible(false);

	UpdateDeceivedGhostPresentation();

	if (!bSimulationPresentationActive || !IsValid(ADWorldRuntime))
	{
		ClearExecutionPreview();
		return;
	}

	const FBattleSimulationPreviewData* PreviewData = PreviewDataByWorld.Find(EBattleSimulationWorldType::PlayerActualEnemyDeceived);
	if (PreviewData)
		DisplayExecutionPreview(ADWorldRuntime, *PreviewData);
	else
		ClearExecutionPreview();
}

void UBattleSimulationPresentationController::UpdateDeceivedGhostPresentation()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : DeceivedGhostCharacterMap)
		SynchronizeDeceivedGhost(Pair.Key.Get(), Pair.Value.Get());
}

bool UBattleSimulationPresentationController::CreateDeceivedGhosts(const TArray<ABattleCharacterBase*>& SourceCharacters)
{
	DestroyDeceivedGhosts();

	ABattleSimulationManager* Manager = SimulationManager.Get();
	UWorld* World = IsValid(Manager) ? Manager->GetWorld() : nullptr;
	TSubclassOf<ABattleSimulationCharacter> CharacterClass = IsValid(Manager) ? Manager->GetSimulationCharacterClass() : nullptr;

	if (!World || !CharacterClass)
		return false;

	for (ABattleCharacterBase* SourceCharacter : SourceCharacters)
	{
		if (!IsValid(SourceCharacter))
		{
			DestroyDeceivedGhosts();
			return false;
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = Manager;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParameters.ObjectFlags |= RF_Transient;

		ABattleSimulationCharacter* GhostCharacter = World->SpawnActor<ABattleSimulationCharacter>(CharacterClass, SourceCharacter->GetActorTransform(), SpawnParameters);
		if (!GhostCharacter)
		{
			DestroyDeceivedGhosts();
			return false;
		}

		UMaterialInterface* GhostMaterial = Manager->GetDeceivedGhostMaterial(SourceCharacter->IsA<ABattleCharacter_Player>());
		GhostCharacter->InitializeFromCharacter(SourceCharacter, GhostMaterial);
		GhostCharacter->SetActorEnableCollision(false);
		GhostCharacter->SetCanBeDamaged(false);
		GhostCharacter->SetActorHiddenInGame(true);
		DeceivedGhostCharacterMap.Add(SourceCharacter, GhostCharacter);
	}

	return true;
}

void UBattleSimulationPresentationController::DestroyDeceivedGhosts()
{
	for (const TPair<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>>& Pair : DeceivedGhostCharacterMap)
	{
		if (IsValid(Pair.Value.Get()))
			Pair.Value->Destroy();
	}

	DeceivedGhostCharacterMap.Empty();
}

void UBattleSimulationPresentationController::SynchronizeDeceivedGhost(ABattleCharacterBase* SourceCharacter, ABattleSimulationCharacter* GhostCharacter)
{
	if (!IsValid(SourceCharacter) || !IsValid(GhostCharacter) || !bSimulationPresentationActive)
	{
		if (IsValid(GhostCharacter))
			GhostCharacter->SetActorHiddenInGame(true);

		return;
	}

	ABattleSimulationManager* Manager = SimulationManager.Get();
	if (!IsValid(Manager))
	{
		GhostCharacter->SetActorHiddenInGame(true);
		return;
	}

	UBattleSimulationWorldRuntime* ADWorldRuntime = Manager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerActualEnemyDeceived);
	UBattleSimulationWorldRuntime* DDWorldRuntime = Manager->GetSimulationWorldRuntime(EBattleSimulationWorldType::PlayerDeceivedEnemyDeceived);
	ABattleSimulationCharacter* ADCharacter = IsValid(ADWorldRuntime) ? ADWorldRuntime->GetSimulationCharacter(SourceCharacter) : nullptr;
	ABattleSimulationCharacter* DDCharacter = IsValid(DDWorldRuntime) ? DDWorldRuntime->GetSimulationCharacter(SourceCharacter) : nullptr;

	if (!IsValid(ADCharacter) || !IsValid(DDCharacter))
	{
		GhostCharacter->SetActorHiddenInGame(true);
		return;
	}

	const bool bLocationDiffers = !ADCharacter->GetActorLocation().Equals(DDCharacter->GetActorLocation(), 1.0f);
	const bool bRotationDiffers = !ADCharacter->GetActorRotation().Equals(DDCharacter->GetActorRotation(), 1.0f);

	GhostCharacter->SetActorTransform(DDCharacter->GetActorTransform());
	GhostCharacter->SetActorHiddenInGame(!bLocationDiffers && !bRotationDiffers);
}

void UBattleSimulationPresentationController::DisplayExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPreviewData& PreviewData)
{
	ClearExecutionPreview();
	ABattleCharacterBase* RuntimeAttacker = PreviewData.Action.Attacker.Get();
	const int32 StepCount = PreviewData.Action.Card->TargetingData.Steps.Num();
	for (int32 StepIndex = 0; StepIndex < StepCount; ++StepIndex)
	{
		const FTargetingStepCardData* StepData = PreviewData.Action.Card->TargetingData.GetStep(StepIndex);
		if (!StepData || !PreviewData.TargetingResult.GetStep(StepIndex))
			continue;
		AddExecutionStepPreview(WorldRuntime, RuntimeAttacker, PreviewData.Action, StepIndex, *StepData, PreviewData.TargetingResult);
	}
}

void UBattleSimulationPresentationController::ClearExecutionPreview()
{
	if (TargetingPresentationController)
		TargetingPresentationController->ClearStepPreviews();
}

void UBattleSimulationPresentationController::AddExecutionStepPreview(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FTargetingResult& TargetingResult)
{
	const FTargetingPhasePresentationSettings& Settings = StepData.Presentation.Phases.Simulation;
	if (!Settings.HasAnyPresentation() || !TargetingPresentationController || !IsValid(SimulationManager.Get()))
		return;

	const FTargetingStepResult* StepResult = TargetingResult.GetStep(StepIndex);
	if (!StepResult)
		return;

	FTargetingPreviewContext PreviewContext;
	PreviewContext.SourceCharacter = RuntimeAttacker;
	PreviewContext.GridManager = SimulationManager->GetBattleGridManager();
	PreviewContext.StepData = &StepData;
	PreviewContext.TargetingStep = StepResult;
	PreviewContext.PresentationSettings = &Settings;
	TargetingPresentationController->AddStepPreview(PreviewContext);
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
	if (!World || !PostProcessVolumeClass)
		return false;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = SimulationManager;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.ObjectFlags |= RF_Transient;
	SimulationPostProcessVolume = World->SpawnActor<ABattleSimulationPostProcessVolume>(PostProcessVolumeClass, SimulationManager->GetActorTransform(), SpawnParameters);
	if (!SimulationPostProcessVolume)
		return false;
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
