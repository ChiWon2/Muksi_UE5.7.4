#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationWorldRuntime.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleSimulationCharacter;
class ABattleSimulationManager;
class UBattleSimulationWorldRuntime;
class UMuksiBattleCardDataAsset;
class UBattleActionExecutor;
struct FBattleExecutionEntry;
struct FResolvedTargeting;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSimulationWorldExchangeFinished, UBattleSimulationWorldRuntime*, int32);

/**
 * AD / DD / DA 중 할당받은 Simulation World 하나의 Persistent Runtime을 소유한다.
 * BattleStart에 Character, 공용 Grid의 WorldState, ActionExecutor를 준비하고 RoundStart에 AA 상태를 복사해 재사용한다.
 * 다른 Simulation World의 생성, 표시 전환, 동시 완료 집계와 AA Action Commit에는 관여하지 않는다.
 */
UCLASS()
class MUKSI_API UBattleSimulationWorldRuntime : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleSimulationManager* InSimulationManager, EBattleSimulationWorldType InWorldType);
	void Shutdown();
	bool PrepareSimulationRuntime(ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool ResetFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool PrepareExchange(int32 ExchangeIndex, const FBattleAction& PlayerAction, const FBattleAction& EnemyAction);
	bool ExecuteCurrentExchange();
	void StopSimulation();
	void SetCharactersVisible(bool bVisible);

	bool IsSimulationRunning() const;
	bool IsSimulationRuntimeReady() const;
	EBattleSimulationState GetSimulationState() const { return SimulationState; }
	int32 GetCurrentExchangeIndex() const { return CurrentExchangeIndex; }
	EBattleSimulationWorldType GetWorldType() const { return WorldType; }
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;
	ABattleCharacterBase* GetSourceCharacter(const ABattleSimulationCharacter* SimulationCharacter) const;

	FOnBattleSimulationWorldExchangeFinished ExchangeFinishedDelegate;

protected:
	virtual void BeginDestroy() override;

private:
	UMuksiBattleCardDataAsset* GetExecutionOverride(const FBattleAction& Action) const;
	bool CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateSimulationExecutionEnvironment(ABattleGridManager* InSourceGridManager);
	bool RebuildWorldGridState(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CanReuseSimulationRuntime(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters) const;
	bool ResetSimulationRuntimeFromActualBattleState(ABattleGridManager* InSourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool TryExecuteCurrentExchange();
	bool BuildSimulationSequenceRequest(const FBattleAction& Action, FBattleSequenceRequest& OutRequest) const;
	bool ExecuteSimulationAction(const FBattleSequenceRequest& Request);
	void ClearSimulationActionPresentation();
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationSequenceFinished();
	void FinishCurrentExchange();
	void DestroySimulationRuntime();
	void ResetSimulationRuntime();
	void SetSimulationState(EBattleSimulationState NewState);
	int32 GetMaxExchangeCount() const;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(Transient)
	EBattleSimulationState SimulationState = EBattleSimulationState::Idle;

	UPROPERTY(Transient)
	int32 CurrentExchangeIndex = INDEX_NONE;

	UPROPERTY(Transient)
	FBattleSequenceRequest PlayerActionRequest;

	UPROPERTY(Transient)
	FBattleSequenceRequest EnemyActionRequest;

	// 별도 GridManager를 소유하지 않고 BattleManager의 공용 Grid를 참조한다.
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleActionExecutor> ActionExecutor = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;
};
