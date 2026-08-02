#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleSequenceManager;
class ABattleSimulationCharacter;
class ABattleSimulationPostProcessVolume;
class UMuksiBattleCardDataAsset;
class UMaterialInterface;
struct FBattleSequenceRequest;
struct FBattleExecutionEntry;
struct FResolvedTargeting;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationExchangeFinished, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleSimulationFinished);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationActionStarted, const FBattleAction&);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnSimulationExecutionStarted, const FBattleAction&, const FBattleExecutionEntry&, int32, const FResolvedTargeting&);
DECLARE_MULTICAST_DELEGATE(FOnSimulationActionFinished);

UCLASS()
class MUKSI_API ABattleSimulationManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationManager();
	FOnSimulationExchangeFinished OnSimulationExchangeFinished;
	FOnBattleSimulationFinished OnBattleSimulationFinished;
	FOnSimulationActionStarted OnSimulationActionStarted;
	FOnSimulationExecutionStarted OnSimulationExecutionStarted;
	FOnSimulationActionFinished OnSimulationActionFinished;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	EBattleSimulationState GetSimulationState() const { return SimulationState; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	int32 GetCurrentExchangeIndex() const { return CurrentExchange.ExchangeIndex; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	bool IsSimulationRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	bool StartSimulation(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	bool SetPlayerAction(const FBattleAction& PlayerAction, UMuksiBattleCardDataAsset* SimulationCardOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	bool SetEnemyAction(const FBattleAction& EnemyAction, UMuksiBattleCardDataAsset* SimulationCardOverride = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	bool ExecuteCurrentExchange();

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	void StopSimulation();

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleGridManager* GetSimulationGridManager() const { return SourceGridManager; }

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const;

	const FBattleSimulationExchange& GetCurrentExchange() const { return CurrentExchange; }
	const TArray<FBattleAction>& GetSequenceActionQueue() const { return SequenceActionQueue; }

public:
	bool CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateSimulationExecutionEnvironment(ABattleGridManager* SourceGridManager);
	bool CreateSimulationPostProcess();
	void DestroySimulationPostProcess();
	void HideSourceCharacters();
	void RestoreSourceCharacters();
	bool TryExecuteCurrentExchange();
	bool ExecuteSimulationAction(const FBattleSimulationActionPlan& ActionPlan);
	bool BuildSimulationSequenceRequest(const FBattleSimulationActionPlan& ActionPlan, FBattleSequenceRequest& OutRequest) const;
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationSequenceFinished();
	void FinishCurrentExchange();
	void DestroySimulationRuntime();
	void ResetSimulationRuntime();
	void SetSimulationState(EBattleSimulationState NewState);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	EBattleSimulationState SimulationState = EBattleSimulationState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	FBattleSimulationExchange CurrentExchange;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TArray<FBattleAction> SequenceActionQueue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Rule", meta = (ClampMin = "1"))
	int32 MaxExchangeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation")
	TSubclassOf<ABattleSimulationCharacter> SimulationCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> PlayerSimulationMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|Material")
	TObjectPtr<UMaterialInterface> EnemySimulationMaterial = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> SourceGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SimulationSequenceManager = nullptr;

	// Disabled by default for clearer simulation testing.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	bool bEnableSimulationPostProcess = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess", meta = (EditCondition = "bEnableSimulationPostProcess"))
	TSubclassOf<ABattleSimulationPostProcessVolume> SimulationPostProcessVolumeClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;
};
