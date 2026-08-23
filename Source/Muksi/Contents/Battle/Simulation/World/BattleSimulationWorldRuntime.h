#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "BattleSimulationWorldRuntime.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class ABattleSimulationCharacter;
class ABattleSimulationManager;
class UBattleSimulationWorldRuntime;
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
	bool Initialize(ABattleSimulationManager* InSimulationManager, EBattleSimulationWorldType InWorldType, ABattleGridManager* SourceGridManager, const TArray<ABattleCharacterBase*>& SourceCharacters);
	void Shutdown();
	bool ResetFromActualBattleState(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool PrepareExchange(int32 ExchangeIndex, const FBattleAction& PlayerAction, const FBattleAction& EnemyAction);
	bool ExecuteCurrentExchange();
	void SetCharactersVisible(bool bVisible);

	bool IsSimulationRunning() const;
	EBattleSimulationState GetSimulationState() const { return SimulationState; }
	EBattleSimulationWorldType GetWorldType() const { return WorldType; }
	ABattleSimulationCharacter* GetSimulationCharacter(const ABattleCharacterBase* SourceCharacter) const;

	FOnBattleSimulationWorldExchangeFinished ExchangeFinishedDelegate;

protected:
	virtual void BeginDestroy() override;

private:
	bool IsSimulationRuntimeInitialized() const;
	bool CreateSimulationCharacters(const TArray<ABattleCharacterBase*>& SourceCharacters);
	bool CreateActionExecutor(ABattleGridManager* InSourceGridManager);
	bool ResetGridStateFromActual();
	bool BuildSimulationAction(const FBattleAction& Action, FBattleAction& OutAction) const;
	bool ExecuteSimulationAction(const FBattleAction& Action);
	void HandleSimulationExecutionStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FResolvedTargeting& ResolvedTargeting);
	void HandleSimulationSequenceFinished();
	void FinishCurrentExchange();
	void AbortSimulation();
	void DestroySimulationRuntime();

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleSimulationManager> SimulationManager = nullptr;

	UPROPERTY(Transient)
	EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	UPROPERTY(Transient)
	EBattleSimulationState SimulationState = EBattleSimulationState::Idle;

	UPROPERTY(Transient)
	FBattleAction PreparedPlayerAction;

	UPROPERTY(Transient)
	FBattleAction PreparedEnemyAction;

	UPROPERTY(Transient)
	TObjectPtr<UBattleActionExecutor> ActionExecutor = nullptr;

	UPROPERTY(Transient)
	TMap<TObjectPtr<ABattleCharacterBase>, TObjectPtr<ABattleSimulationCharacter>> SimulationCharacterMap;
};
