#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInterface.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class ABattleSimulationCharacter;
class ABattleSimulationPostProcessVolume;
class ABattleDDPresentationActor;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleSimulationPresentationController;
class UBattleSimulationWorldRuntime;
struct FBattleAction;
struct FTargetingResult;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulationTimeScaleChanged, float, TimeScale);

/**
 * Round Simulation 전체를 조율한다.
 * AD / DD / DA WorldRuntime을 생성하고 동시에 실행하며 완료를 집계한다.
 * AD만 실제 Simulation Presentation으로 표시하고, DD는 PresentationController의 경량 DD Presentation Actor로 위치와 방향만 표현한다.
 * World별 시간 배율과 완료 집계 및 AA Action Commit을 관리한다.
 * 개별 World의 Character / Grid 복제와 Sequence 실행은 UBattleSimulationWorldRuntime이 담당한다.
 */
UCLASS()
class MUKSI_API ABattleSimulationManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationManager();
	virtual void Tick(float DeltaSeconds) override;
	bool InitializeBattleFlow(ABattleManager* InBattleManager);
	ABattleManager* GetBattleManager() const { return BattleManager.Get(); }
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Presentation")
	UBattleSimulationPresentationController* GetPresentationController() const { return PresentationController.Get(); }
	TSubclassOf<ABattleSimulationCharacter> GetSimulationCharacterClass() const { return SimulationCharacterClass; }
	TSubclassOf<ABattleDDPresentationActor> GetDDPresentationActorClass() const { return DDPresentationActorClass; }
	UBattleSimulationWorldRuntime* GetSimulationWorldRuntime(EBattleSimulationWorldType WorldType) const;
	ABattleCharacterBase* GetCharacterForWorld(const ABattleCharacterBase* SourceCharacter, EBattleSimulationWorldType WorldType) const;
	ABattleGridManager* GetBattleGridManager() const;
	bool IsSimulationPostProcessEnabled() const { return bEnableSimulationPostProcess; }
	UMaterialInterface* GetSimulationPostProcessMaterial() const { return SimulationPostProcessMaterial.Get(); }
	float GetSimulationPostProcessBlendWeight() const { return SimulationPostProcessBlendWeight; }
	bool ShouldLockSimulationAutoExposure() const { return bLockSimulationAutoExposure; }
	float GetSimulationFixedExposure() const { return SimulationFixedExposure; }
	float GetFastForwardSimulationTimeScale() const { return FastForwardSimulationTimeScale; }
	float GetCurrentSimulationTimeScale() const { return CurrentSimulationTimeScale; }

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	EBattleSimulationState GetSimulationState() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	bool IsSimulationRunning() const;

	// Hidden Simulation World 중 하나라도 배속 중이면 FastForwardSimulationTimeScale, 아니면 1.0을 전달한다.
	UPROPERTY(BlueprintAssignable, Category = "Battle|Simulation|Time")
	FOnSimulationTimeScaleChanged SimulationTimeScaleChangedDelegate;

private:
	friend class UBattleSimulationWorldRuntime;
	friend class UBattleSimulationPresentationController;

	UFUNCTION()
	void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	UFUNCTION()
	void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	void ExecuteBattleStart();
	void ExecuteRoundStart();
	void ExecuteSimulationSequence();
	void CompletePhaseExecution(EBattlePhase FinishedPhase);
	void TryCompleteSimulationSequencePhase(int32 FinishedExchangeIndex);

	bool CreateSimulationWorldRuntimes(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateSimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime, EBattleSimulationWorldType WorldType, ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	void DestroySimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime);
	void DestroySimulationWorldRuntimes();
	TArray<UBattleSimulationWorldRuntime*> GetSimulationWorldRuntimes() const;
	bool InitializeBattleSimulation();
	bool InitializeRoundSimulation();
	bool ResetSimulationWorldsFromActualBattleState(const TArray<ABattleCharacterBase*>& SourceCharacters);
	void DeactivateRoundSimulation();
	void StopSimulation();
	bool CreatePresentationController();

	bool IsManagedSimulationRuntime(const UBattleSimulationWorldRuntime* WorldRuntime) const;


	void NotifySimulationWorldExecutionStarted(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void NotifySimulationWorldExchangeCompleted(UBattleSimulationWorldRuntime* WorldRuntime, int32 ExchangeIndex, bool bSucceeded);
	void FinalizeCurrentExchangeSimulation(int32 FinishedExchangeIndex);
	bool CommitActualExchangeActions(int32 ExchangeIndex);
	bool ValidateActualExchangeAction(const FBattleAction& Action, int32 ExchangeIndex, bool bExpectedPlayerAction) const;
	bool PrepareCurrentExchangeSimulation();
	bool StartCurrentExchangeSimulation();
	void RefreshSimulationWorldTimeScales();
	void ResetSimulationWorldTimeScales();
	void ResetExchangeCompletionTracking(int32 ExchangeIndex);
	void ClearExchangeCompletionTracking();
	bool IsCurrentExchangeCompletionTrackingComplete() const;
	bool AreAllSimulationWorldsCompleted() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TSubclassOf<ABattleSimulationCharacter> SimulationCharacterClass;

	// Lightweight AD-side marker for the position resolved by the DD world.
	// This class should not depend on the source character presentation hierarchy.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Presentation")
	TSubclassOf<ABattleDDPresentationActor> DDPresentationActorClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleSimulationWorldRuntime> ADWorldRuntime = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleSimulationWorldRuntime> DDWorldRuntime = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleSimulationWorldRuntime> DAWorldRuntime = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleSimulationPresentationController> PresentationController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Time", meta = (ClampMin = "1.0"))
	float FastForwardSimulationTimeScale = 3.0f;

	float CurrentSimulationTimeScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	bool bEnableSimulationPostProcess = false;

	// Configured on the BattleSimulationManager BP/defaults.
	// The spawned PostProcessVolume receives this material at runtime.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess"))
	TObjectPtr<UMaterialInterface> SimulationPostProcessMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess", ClampMin = "0.0", ClampMax = "1.0"))
	float SimulationPostProcessBlendWeight = 1.0f;

	// Locks eye adaptation while the simulation post process is active.
	// The same value is applied to the volume's Auto Exposure min/max, which disables adaptation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess"))
	bool bLockSimulationAutoExposure = true;

	// In projects using the extended luminance range this is displayed conceptually as EV100.
	// Keeping min/max equal is what disables automatic adaptation; 0 is a neutral test default.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess && bLockSimulationAutoExposure", EditConditionHides))
	float SimulationFixedExposure = 0.0f;

	TSet<EBattleSimulationWorldType> CompletedWorldTypesForCurrentExchange;
	int32 CompletionTrackingExchangeIndex = INDEX_NONE;
	UPROPERTY(Transient)
	TObjectPtr<UBattlePhaseTask> PhaseExecutionTask = nullptr;
};
