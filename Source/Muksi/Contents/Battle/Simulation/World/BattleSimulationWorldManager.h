#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedTargeting.h"
#include "BattleSimulationWorldManager.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleManager;
class ABattleSequenceManager;
class ABattleSimulationCharacter;
class ABattleSimulationWorldManager;
class UMuksiBattleCardDataAsset;
class UMaterialInterface;
class UTargetingPresentationController;
struct FBattleSequenceRequest;
struct FBattleExecutionEntry;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSimulationWorldStateChanged, ABattleSimulationWorldManager*, EBattleSimulationState);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBattleSimulationWorldExchangeFinished, ABattleSimulationWorldManager*, int32, bool, const FBattleSimulationExchange&);

/**
 * AD / DD / DA 중 할당받은 Simulation World 하나의 Runtime을 소유한다.
 * AA 상태에서 Character와 Grid를 복제하고 해당 World 정책으로 Targeting과 Sequence를 실행한다.
 * 다른 Simulation World의 생성, 표시 전환, 동시 완료 집계와 AA Action Commit에는 관여하지 않는다.
 */
UCLASS()
class MUKSI_API ABattleSimulationWorldManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSimulationWorldManager();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	bool InitializeWorld(const FBattleSimulationWorldPolicy& InWorldPolicy, ABattleManager* InBattleManager, int32 InMaxExchangeCount, TSubclassOf<ABattleSimulationCharacter> InSimulationCharacterClass, UMaterialInterface* InPlayerSimulationMaterial, UMaterialInterface* InEnemySimulationMaterial);
	bool ResetFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool PrepareExchange(int32 ExchangeIndex, const FBattleAction& PlayerAction, const FBattleAction& EnemyAction);
	bool ExecuteCurrentExchange();
	void StopSimulation();
	void ClearRuntimeSimulationPreview();
	void SetWorldVisible(bool bVisible);

	bool IsSimulationRunning() const;
	bool IsWorldVisible() const { return bWorldVisible; }
	EBattleSimulationState GetSimulationState() const { return SimulationState; }
	const FBattleSimulationExchange& GetCurrentExchange() const { return CurrentExchange; }
	const FBattleSimulationWorldPolicy& GetWorldPolicy() const { return WorldPolicy; }
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;
	ABattleCharacterBase* GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const;
	ABattleGridManager* GetSimulationGridManager() const { return SimulationGridManager; }
	ABattleGridManager* GetSourceGridManager() const { return SourceGridManager; }

	FOnBattleSimulationWorldStateChanged StateChangedDelegate;
	FOnBattleSimulationWorldExchangeFinished ExchangeFinishedDelegate;

private:
	bool SetPlayerAction(const FBattleAction& PlayerAction);
	bool SetEnemyAction(const FBattleAction& EnemyAction);
	UMuksiBattleCardDataAsset* GetExecutionOverride(const FBattleAction& Action, EBattleSimulationKnowledge Knowledge) const;
	bool CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager);
	bool TryExecuteCurrentExchange();
	bool ExecuteSimulationAction(const FBattleSimulationActionPlan& ActionPlan);
	bool BuildSimulationSequenceRequest(const FBattleSimulationActionPlan& ActionPlan, FBattleSequenceRequest& OutRequest) const;
	void HandleSimulationActionStarted(const FBattleAction& Action);
	void HandleSimulationActionFinished();
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationSequenceFinished();
	void RefreshSimulationTargetingPresentation(const FBattleAction& Action, const FResolvedTargeting& ExecutionResolvedTargeting);
	bool ResolveSimulationActionTargetingThroughStep(const FBattleAction& Action, int32 LastStepIndex, FResolvedTargeting& OutResolvedTargeting) const;
	void FinishCurrentExchange();
	void DestroySimulationRuntime();
	void ResetSimulationRuntime();
	void SetSimulationState(EBattleSimulationState NewState);

private:
	UPROPERTY(Transient)
	FBattleSimulationWorldPolicy WorldPolicy;

	UPROPERTY(Transient)
	EBattleSimulationState SimulationState = EBattleSimulationState::Idle;

	UPROPERTY(Transient)
	FBattleSimulationExchange CurrentExchange;

	UPROPERTY(Transient)
	int32 MaxExchangeCount = 3;

	UPROPERTY(Transient)
	TSubclassOf<ABattleSimulationCharacter> SimulationCharacterClass;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> PlayerSimulationMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInterface> EnemySimulationMaterial = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> SourceGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> SimulationGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleSequenceManager> SimulationSequenceManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UTargetingPresentationController> SimulationTargetingPresentationController = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;

	UPROPERTY(Transient)
	TArray<TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterOrder;

	UPROPERTY(Transient)
	FBattleAction CachedPresentationAction;

	UPROPERTY(Transient)
	FResolvedTargeting CachedPresentationResolvedTargeting;

	bool bHasCachedPresentation = false;
	bool bWorldVisible = true;
};
