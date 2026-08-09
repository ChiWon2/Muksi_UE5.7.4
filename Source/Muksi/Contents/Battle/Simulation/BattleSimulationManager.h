#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class ABattleSequenceManager;
class ABattleSimulationCharacter;
class UBattleRuntimeContext;
class ABattleSimulationPostProcessVolume;
class UMuksiBattleCardDataAsset;
class UMaterialInterface;
class UTargetingPresentationController;
struct FBattleSequenceRequest;
struct FBattleExecutionEntry;
struct FResolvedTargeting;

/**
 * Round Simulation Runtime, Exchange Action 실행과 Simulation Preview를 담당한다.
 * Phase 순서는 결정하지 않으며 SimulationSequence 완료만 BattleManager에 통지한다.
 */
UCLASS()
class MUKSI_API ABattleSimulationManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	EBattleSimulationState GetSimulationState() const { return SimulationState; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	int32 GetCurrentExchangeIndex() const { return CurrentExchange.ExchangeIndex; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	bool IsSimulationRunning() const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleGridManager* GetSimulationGridManager() const { return SourceGridManager; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const;

	const FBattleSimulationExchange& GetCurrentExchange() const { return CurrentExchange; }

private:
	bool TryBindBattleFlow();
	void BindBattleFlowDeferred();

	UFUNCTION()
	void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);
	void HandleBattlePhaseUIFinished(EBattlePhase OldPhase, EBattlePhase NewPhase);

	bool InitializeRoundSimulation();
	bool PrepareCurrentExchangeSimulation();
	bool StartCurrentExchangeSimulation();
	void NotifySimulationPhaseFinished(int32 FinishedExchangeIndex);

	bool StartSimulation(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool SetPlayerAction(const FBattleAction& PlayerAction, UMuksiBattleCardDataAsset* SimulationCardOverride = nullptr);
	bool SetEnemyAction(const FBattleAction& EnemyAction, UMuksiBattleCardDataAsset* SimulationCardOverride = nullptr);
	UMuksiBattleCardDataAsset* GetSimulationExecutionOverride(const FBattleAction& Action) const;
	bool ExecuteCurrentExchange();
	void StopSimulation();

	bool CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager);
	bool CreateSimulationPostProcess();
	void DestroySimulationPostProcess();
	void HideSourceCharacters();
	void RestoreSourceCharacters();
	bool TryExecuteCurrentExchange();
	bool ExecuteSimulationAction(const FBattleSimulationActionPlan& ActionPlan);
	bool BuildSimulationSequenceRequest(const FBattleSimulationActionPlan& ActionPlan, FBattleSequenceRequest& OutRequest) const;
	void HandleSimulationActionStarted(const FBattleAction& Action);
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationActionFinished();
	void HandleSimulationSequenceFinished();
	void RefreshSimulationTargetingPresentation(const FBattleAction& Action, const FResolvedTargeting& ExecutionResolvedTargeting);
	bool ResolveSimulationActionTargetingThroughStep(const FBattleAction& Action, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting) const;
	void ClearRuntimeSimulationPreview();
	void FinishCurrentExchange();
	void DestroySimulationRuntime();
	void ResetSimulationRuntime();
	void SetSimulationState(EBattleSimulationState NewState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	EBattleSimulationState SimulationState = EBattleSimulationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	FBattleSimulationExchange CurrentExchange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Rule")
	int32 MaxExchangeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TSubclassOf<ABattleSimulationCharacter> SimulationCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> PlayerSimulationMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> EnemySimulationMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleRuntimeContext> BattleRuntimeContext = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> SourceGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SimulationSequenceManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> SimulationTargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	// Disabled by default for clearer simulation testing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	bool bEnableSimulationPostProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess"))
	TSubclassOf<ABattleSimulationPostProcessVolume> SimulationPostProcessVolumeClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;

	FTimerHandle BattleFlowBindingTimerHandle;
};
