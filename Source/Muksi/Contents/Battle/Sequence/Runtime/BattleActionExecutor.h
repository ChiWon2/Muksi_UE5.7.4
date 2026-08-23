#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleActionExecutor.generated.h"

class ABattleGridManager;
class ABattleManager;
class UBattleExecutionRunner;
class UBattleSequenceExecutionEnvironment;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;

DECLARE_MULTICAST_DELEGATE(FOnBattleActionExecutorFinished);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBattleActionExecutorEntryStarted, const FBattleAction&, const FBattleExecutionEntry&, int32, const FResolvedTargeting&);

UCLASS()
class MUKSI_API UBattleActionExecutor : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleManager* InBattleManager, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType);
	bool ExecuteAction(const FBattleAction& Action);
	void Stop();
	bool IsRunning() const { return bRunning; }

	FOnBattleActionExecutorFinished FinishedDelegate;
	FOnBattleActionExecutorEntryStarted EntryStartedDelegate;

private:
	bool ValidateAction(const FBattleAction& Action) const;
	UMuksiBattleCardDataAsset* ResolveExecutionCard(const FBattleAction& Action) const;
	bool InitializeExecutionEnvironment();
	bool BindAttackerNotify();
	void UnbindAttackerNotify();
	void StartMainExecutionChain();
	void StartNotifyExecutionChains(FName NotifyKey);
	void StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);
	FBattleExecutionContext MakeExecutionContext(FName NotifyKey) const;
	void HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext);
	void HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
	void TryFinish();
	void Finish();
	void ResetRuntime();

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	FBattleAction CurrentAction;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CurrentExecutionCard = nullptr;

	UPROPERTY(Transient)
	FResolvedTargeting CurrentResolvedTargeting;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> AttackerAnimationComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBattleExecutionRunner>> ActiveExecutionRunners;

	UPROPERTY(Transient)
	TObjectPtr<UBattleSequenceExecutionEnvironment> ExecutionEnvironment = nullptr;

	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
	EBattleExecutionMode ExecutionMode = EBattleExecutionMode::Sequence;
	bool bRunning = false;
};
