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
struct FBattleSimulationPreviewData
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
	float GetSimulationTimeScale() const;

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
	void ExitSimulationPresentation(bool bClearPreviewData);
	void ClearAllPreviewData();
	void UpdatePreviewData(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void RemovePreviewData(UBattleSimulationWorldRuntime* WorldRuntime);


private:
	void SynchronizeSimulationPresentation();
	void DisplayExecutionPreview(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleSimulationPreviewData& PreviewData);
	void ClearExecutionPreview();
	void AddExecutionStepPreview(UBattleSimulationWorldRuntime* WorldRuntime, ABattleCharacterBase* RuntimeAttacker, const FBattleAction& Action, int32 StepIndex, const FTargetingStepCardData& StepData, const FTargetingResult& TargetingResult);
	bool CreateSimulationPostProcess();
	void DestroySimulationPostProcess();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<EBattleSimulationWorldType, FBattleSimulationPreviewData> PreviewDataByWorld;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	EBattlePlayerSimulationView PlayerSimulationView = EBattlePlayerSimulationView::ActualSelf;
	bool bSimulationPresentationActive = false;
	bool bPlayerSimulationViewChangeLocked = false;
};
