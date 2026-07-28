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

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSimulationExchangeFinished, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleSimulationFinished);

UCLASS()
class MUKSI_API ABattleSimulationManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationManager();
	FOnSimulationExchangeFinished OnSimulationExchangeFinished;
	FOnBattleSimulationFinished OnBattleSimulationFinished;

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
	bool SetPlayerAction(const FBattleAction& PlayerAction);

	UFUNCTION(BlueprintCallable, Category = "Battle|Simulation")
	bool SetEnemyAction(const FBattleAction& EnemyAction);

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
	bool ExecuteSimulationAction(const FBattleAction& SourceAction);
	bool ConvertToSimulationAction(const FBattleAction& SourceAction, FBattleAction& OutSimulationAction) const;
	void ConvertTargetCharacters(FTargetingResult& TargetingResult) const;
	void ConvertToSourceTargetingResult(FTargetingResult& TargetingResult) const;
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

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, bool> SourceCharacterHiddenStates;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> SourceGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SimulationSequenceManager = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Simulation|PostProcess")
	TSubclassOf<ABattleSimulationPostProcessVolume> SimulationPostProcessVolumeClass;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationPostProcessVolume> SimulationPostProcessVolume = nullptr;
};