#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "UObject/Object.h"
#include "BattleSimulationPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleSimulationManager;
class ABattleSimulationPostProcessVolume;
class UBattleSimulationWorldRuntime;
class UTargetingPresentationController;
struct FTargetingStepCardData;

USTRUCT()
struct FBattleSimulationExecutionResult
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FBattleAction Action;

	UPROPERTY(Transient)
	FTargetingResult TargetingResult;

};

UCLASS(BlueprintType)
class MUKSI_API UBattleSimulationPresentationController : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager, UTargetingPresentationController* InTargetingPresentationController);
	void Shutdown();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	EBattlePlayerSimulationView GetPlayerSimulationView() const { return PlayerSimulationView; }
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	bool CanChangePlayerSimulationView() const { return bSimulationPresentationActive && !bPlayerSimulationViewChangeLocked; }
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Time")
	float GetSimulationTimeScale() const { return CurrentSimulationTimeScale; }

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool RequestPlayerSimulationView(EBattlePlayerSimulationView NewView);
	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool TogglePlayerSimulationView();
	void SetPlayerSimulationView(EBattlePlayerSimulationView NewView);
	void SetPlayerSimulationViewChangeLocked(bool bLocked);

	UBattleSimulationWorldRuntime* GetPlayerPresentationWorldRuntime() const;
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	ABattleCharacterBase* GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	bool EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void ExitSimulationPresentation(bool bClearExecutionResults);
	void ClearAllExecutionResults();
	void UpdateExecutionResult(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void RemoveExecutionResult(UBattleSimulationWorldRuntime* WorldRuntime);

	void StartSimulationFastForward();
	void StopSimulationFastForward();

private:
	void SynchronizeSimulationPresentation();
	void DisplayExecutionResult(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationExecutionResult& ExecutionResult);
	void ClearDisplayedExecutionResult();
	void AppendStepPreview(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FTargetingResult& TargetingResult);
	bool CreateSimulationPostProcess();
	void DestroySimulationPostProcess();
	void CaptureSimulationTimeScaleBaseline();
	void SetSimulationTimeScale(float NewTimeScale);
	void RestoreSimulationTimeScale();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<EBattleSimulationWorldType, FBattleSimulationExecutionResult> ExecutionResultsByWorld;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	EBattlePlayerSimulationView PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	float CurrentSimulationTimeScale = 1.0f;
	float CapturedGlobalTimeDilation = 1.0f;
	bool bSimulationPresentationActive = false;
	bool bPlayerSimulationViewChangeLocked = false;
	bool bHasCapturedGlobalTimeDilation = false;
};
