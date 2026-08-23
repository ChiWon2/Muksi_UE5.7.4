#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "UObject/Object.h"
#include "BattleSimulationPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleSimulationManager;
class ABattleSimulationPostProcessVolume;
class UBattleSimulationWorldRuntime;
class UTargetingPresentationController;
struct FHexOffsetCoord;
struct FTargetingStepCardData;

USTRUCT()
struct FBattleSimulationPresentationState
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FBattleAction Action;

	UPROPERTY(Transient)
	FResolvedTargeting ResolvedTargeting;

};

UCLASS(BlueprintType)
class MUKSI_API UBattleSimulationPresentationController : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager);
	void Shutdown();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	EBattlePlayerSimulationView GetPlayerSimulationView() const { return PlayerSimulationView; }
	bool IsPlayerSimulationViewAvailable() const { return bPlayerSimulationViewAvailable; }
	bool IsPlayerSimulationViewChangeLocked() const { return bPlayerSimulationViewChangeLocked; }
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	bool CanChangePlayerSimulationView() const { return bPlayerSimulationViewAvailable && !bPlayerSimulationViewChangeLocked; }
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Time")
	float GetSimulationTimeScale() const { return CurrentSimulationTimeScale; }

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool SetPlayerSimulationView(EBattlePlayerSimulationView NewView);
	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool TogglePlayerSimulationView();
	void SetPlayerSimulationViewInternal(EBattlePlayerSimulationView NewView);
	void SetPlayerSimulationViewAvailable(bool bAvailable);
	void SetPlayerSimulationViewChangeLocked(bool bLocked);

	UBattleSimulationWorldRuntime* GetPlayerPresentationWorldRuntime() const;
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	ABattleCharacterBase* GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	bool EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void ExitSimulationPresentation(bool bClearRuntimePreview);
	void ClearRuntimeSimulationPreview();
	void PresentSimulationExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting);
	void ClearSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime);

	void StartSimulationFastForward();
	void StopSimulationFastForward();

private:
	void ApplyPlayerSimulationView();
	void RefreshSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPresentationState& PresentationState);
	void ClearDisplayedExecutionPresentation();
	void PresentSimulationStep(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FResolvedTargeting& StepResolvedTargeting, TArray<FHexOffsetCoord>& OutIndicatorCoords);
	ABattleGridManager* GetGridManager() const;
	ABattleCharacterBase* ResolveRuntimeAttacker(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action) const;
	bool ResolveSimulationStepTargeting(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPresentationState& PresentationState, int32 StepIndex, FResolvedTargeting& OutResolvedTargeting) const;
	void BroadcastPresentationCharacters();
	bool CreateSimulationPostProcess();
	void DestroySimulationPostProcess();
	void HideSourceCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void RestoreSourceCharacters();
	void CaptureSimulationTimeScaleBaseline();
	void SetSimulationTimeScale(float NewTimeScale);
	void RestoreSimulationTimeScale();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<EBattleSimulationWorldType, FBattleSimulationPresentationState> PresentationStates;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	EBattlePlayerSimulationView PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	float CurrentSimulationTimeScale = 1.0f;
	float CapturedGlobalTimeDilation = 1.0f;
	bool bPlayerSimulationViewAvailable = false;
	bool bPlayerSimulationViewChangeLocked = false;
	bool bHasCapturedGlobalTimeDilation = false;
};
