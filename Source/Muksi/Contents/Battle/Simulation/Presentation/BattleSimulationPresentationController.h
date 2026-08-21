#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "UObject/Object.h"
#include "BattleSimulationPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleSimulationManager;
class ABattleSimulationPostProcessVolume;
class ABattleSimulationWorldManager;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationTimeScaleChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationViewChanged, EBattlePlayerSimulationView);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSimulationPresentationAvailabilityChanged, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSimulationPresentationCharactersChanged, ABattleCharacterBase*, ABattleCharacterBase*);

UCLASS()
class MUKSI_API UBattleSimulationPresentationController : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager, ABattleSimulationWorldManager* InADWorldManager, ABattleSimulationWorldManager* InDDWorldManager, ABattleSimulationWorldManager* InDAWorldManager, bool bInEnablePostProcess, TSubclassOf<ABattleSimulationPostProcessVolume> InPostProcessVolumeClass, float InFastForwardTimeScale);
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

	ABattleSimulationWorldManager* GetPlayerPresentationWorldManager() const;
	ABattleSimulationWorldManager* GetPlayerTargetingWorldManager() const;
	ABattleCharacterBase* GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	bool EnterSimulationPresentation(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void ExitSimulationPresentation(bool bClearRuntimePreview);
	void ClearRuntimeSimulationPreview();

	void StartSimulationFastForward();
	void StopSimulationFastForward();

	FOnBattleSimulationPresentationTimeScaleChanged TimeScaleChangedDelegate;
	FOnBattleSimulationPresentationViewChanged ViewChangedDelegate;
	FOnBattleSimulationPresentationAvailabilityChanged AvailabilityChangedDelegate;
	FOnBattleSimulationPresentationCharactersChanged PresentationCharactersChangedDelegate;

private:
	void ApplyPlayerSimulationView();
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
	TObjectPtr<ABattleSimulationWorldManager> ADWorldManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationWorldManager> DDWorldManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationWorldManager> DAWorldManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	UPROPERTY(Transient)
	TSubclassOf<ABattleSimulationPostProcessVolume> SimulationPostProcessVolumeClass;
	EBattlePlayerSimulationView PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	float FastForwardSimulationTimeScale = 3.0f;
	float CurrentSimulationTimeScale = 1.0f;
	float CapturedGlobalTimeDilation = 1.0f;
	bool bEnableSimulationPostProcess = false;
	bool bPlayerSimulationViewAvailable = false;
	bool bPlayerSimulationViewChangeLocked = false;
	bool bHasCapturedGlobalTimeDilation = false;
};
