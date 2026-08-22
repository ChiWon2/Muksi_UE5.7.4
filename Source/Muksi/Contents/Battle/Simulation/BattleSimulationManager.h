#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class ABattleSimulationCharacter;
class ABattleSimulationPostProcessVolume;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleSimulationPresentationController;
class UBattleSimulationWorldRuntime;
class UMaterialInterface;
struct FBattleAction;
struct FResolvedTargeting;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSimulationTimeScaleChanged, float, TimeScale);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSimulationViewChanged, EBattlePlayerSimulationView, View);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSimulationViewAvailabilityChanged, bool, bAvailable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSimulationPresentationCharactersChanged, ABattleCharacterBase*, PlayerCharacter, ABattleCharacterBase*, EnemyCharacter);

/**
 * Round Simulation 전체를 조율한다.
 * AD / DD / DA WorldRuntime을 생성하고 동시에 실행하며 완료를 집계한다.
 * 표시 World와 시간 및 PostProcess는 PresentationController에 위임하고 AA Action만 실제 전투 Queue에 반영한다.
 * 개별 World의 Character / Grid 복제와 Sequence 실행은 UBattleSimulationWorldRuntime이 담당한다.
 */
UCLASS()
class MUKSI_API ABattleSimulationManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationManager();
	bool InitializeBattleFlow(ABattleManager* InBattleManager);
	ABattleManager* GetBattleManager() const { return BattleManager.Get(); }
	TSubclassOf<ABattleSimulationCharacter> GetSimulationCharacterClass() const { return SimulationCharacterClass; }
	UMaterialInterface* GetPlayerSimulationMaterial() const { return PlayerSimulationMaterial.Get(); }
	UMaterialInterface* GetEnemySimulationMaterial() const { return EnemySimulationMaterial.Get(); }
	UBattleSimulationWorldRuntime* GetSimulationWorldRuntime(EBattleSimulationWorldType WorldType) const;
	ABattleGridManager* GetBattleGridManager() const;
	bool IsSimulationPostProcessEnabled() const { return bEnableSimulationPostProcess; }
	TSubclassOf<ABattleSimulationPostProcessVolume> GetSimulationPostProcessVolumeClass() const { return SimulationPostProcessVolumeClass; }
	float GetFastForwardSimulationTimeScale() const { return FastForwardSimulationTimeScale; }
	void PresentSimulationExecution(UBattleSimulationWorldRuntime* WorldRuntime, const FBattleAction& Action, const FResolvedTargeting& ResolvedTargeting);
	void ClearSimulationExecutionPresentation(UBattleSimulationWorldRuntime* WorldRuntime);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	EBattleSimulationState GetSimulationState() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	int32 GetCurrentExchangeIndex() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	bool IsSimulationRunning() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Time")
	float GetSimulationTimeScale() const;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Simulation|Time")
	FOnSimulationTimeScaleChanged SimulationTimeScaleChangedDelegate;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	EBattlePlayerSimulationView GetPlayerSimulationView() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	bool CanChangePlayerSimulationView() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool SetPlayerSimulationView(EBattlePlayerSimulationView NewView);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation|View")
	bool TogglePlayerSimulationView();

	UPROPERTY(BlueprintAssignable, Category = "Battle|Simulation|View")
	FOnPlayerSimulationViewChanged PlayerSimulationViewChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Simulation|View")
	FOnPlayerSimulationViewAvailabilityChanged PlayerSimulationViewAvailabilityChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Battle|Simulation|View")
	FOnSimulationPresentationCharactersChanged PresentationCharactersChangedDelegate;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|View")
	ABattleCharacterBase* GetPresentationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation|Targeting")
	ABattleSimulationCharacter* GetPlayerTargetingSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const;

private:
	UFUNCTION()
	void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	UFUNCTION()
	void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	bool ShouldHandlePhaseEntry(EBattlePhase Phase) const;
	void UpdateSimulationViewForPhaseTransition(EBattlePhase OldPhase, EBattlePhase NewPhase);
	void ExecutePhaseEntryOperation(EBattlePhase NewPhase);
	void PrepareExchangeOrRestartCardSelection();
	void ExecuteBattleStart();
	void ExecuteRoundStart();
	void ExecuteSimulationSequence();
	void CompletePhaseExecution(EBattlePhase FinishedPhase);
	void NotifySimulationPhaseFinished(int32 FinishedExchangeIndex);

	bool EnsureSimulationWorldRuntimes();
	bool EnsureSimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime, EBattleSimulationWorldType WorldType);
	void DestroySimulationWorldRuntime(TObjectPtr<UBattleSimulationWorldRuntime>& InOutWorldRuntime);
	void DestroySimulationWorldRuntimes();
	TArray<UBattleSimulationWorldRuntime*> GetSimulationWorldRuntimes() const;
	void HideSimulationWorlds();
	void StopSimulationWorlds();
	bool PrepareBattleSimulation();
	bool PrepareSimulationWorldRuntimes(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool AreSimulationWorldRuntimesReady() const;
	bool InitializeRoundSimulation();
	void ResetRoundSimulationState();
	bool ResetSimulationWorldsFromActualBattleState(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	void DeactivateRoundSimulation();
	void StopSimulation();
	bool EnsurePresentationController();

	bool IsManagedSimulationRuntime(const UBattleSimulationWorldRuntime* WorldRuntime) const;
	void HandlePresentationTimeScaleChanged(float TimeScale);
	void HandlePresentationViewChanged(EBattlePlayerSimulationView View);
	void HandlePresentationAvailabilityChanged(bool bAvailable);
	void HandlePresentationCharactersChanged(ABattleCharacterBase* PlayerCharacter, ABattleCharacterBase* EnemyCharacter);

	void HandleSimulationWorldExchangeFinished(UBattleSimulationWorldRuntime* WorldRuntime, int32 FinishedExchangeIndex);
	void FinalizeCurrentExchangeSimulation(int32 FinishedExchangeIndex);
	bool CommitActualExchangeActions(int32 ExchangeIndex);
	bool ValidateActualExchangeAction(const FBattleAction& Action, int32 ExchangeIndex, bool bExpectedPlayerAction) const;
	bool PrepareCurrentExchangeSimulation();
	bool StartCurrentExchangeSimulation();
	void RefreshFastForwardForPrimarySimulationWorld();
	void ResetExchangeCompletionBarrier(int32 ExchangeIndex);
	void ClearExchangeCompletionBarrier();
	bool IsExchangeCompletionBarrierSatisfied() const;
	bool AreAllSimulationWorldsCompleted() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TSubclassOf<ABattleSimulationCharacter> SimulationCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> PlayerSimulationMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> EnemySimulationMaterial = nullptr;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	bool bEnableSimulationPostProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess"))
	TSubclassOf<ABattleSimulationPostProcessVolume> SimulationPostProcessVolumeClass;

	TSet<EBattleSimulationWorldType> FinishedWorldTypesForCurrentExchange;
	int32 ExchangeCompletionBarrierIndex = INDEX_NONE;
	UPROPERTY(Transient)
	TObjectPtr<UBattlePhaseTask> PhaseExecutionTask = nullptr;
};
