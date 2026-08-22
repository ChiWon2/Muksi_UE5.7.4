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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationTimeScaleChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationViewChanged, EBattlePlayerSimulationView);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationAvailabilityChanged, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSimulationPresentationCharactersChanged, ABattleCharacterBase*, ABattleCharacterBase*);

UCLASS()
class MUKSI_API UBattleSimulationPresentationController : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager);
	void Shutdown();

	EBattlePlayerSimulationView GetPlayerSimulationView() const { return PlayerSimulationView; }
	bool IsPlayerSimulationViewAvailable() const { return bPlayerSimulationViewAvailable; }
	bool IsPlayerSimulationViewChangeLocked() const { return bPlayerSimulationViewChangeLocked; }
	bool CanChangePlayerSimulationView() const { return bPlayerSimulationViewAvailable && !bPlayerSimulationViewChangeLocked; }
	float GetSimulationTimeScale() const { return CurrentSimulationTimeScale; }

	bool SetPlayerSimulationView(EBattlePlayerSimulationView NewView);
	bool TogglePlayerSimulationView();
	void SetPlayerSimulationViewInternal(EBattlePlayerSimulationView NewView);
	void SetPlayerSimulationViewAvailable(bool bAvailable);
	void SetPlayerSimulationViewChangeLocked(bool bLocked);

	UBattleSimulationWorldRuntime* GetPlayerPresentationWorldRuntime() const;
	UBattleSimulationWorldRuntime* GetPlayerTargetingWorldRuntime() const;
	ABattleCharacterBase* GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	bool EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void ExitSimulationPresentation(bool bClearRuntimePreview);
	void ClearRuntimeSimulationPreview();
	void PresentSimulationExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting);
	void ClearSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime);

	void StartSimulationFastForward();
	void StopSimulationFastForward();

	FOnBattleSimulationPresentationTimeScaleChanged TimeScaleChangedDelegate;
	FOnBattleSimulationPresentationViewChanged ViewChangedDelegate;
	FOnBattleSimulationPresentationAvailabilityChanged AvailabilityChangedDelegate;
	FOnBattleSimulationPresentationCharactersChanged PresentationCharactersChangedDelegate;

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
