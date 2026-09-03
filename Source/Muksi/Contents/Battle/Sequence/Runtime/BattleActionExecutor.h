#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleActionExecutor.generated.h"

class ABattleGridManager;
class ABattleManager;
class UBattleExecutionRunner;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;

DECLARE_DELEGATE(FBattleActionCompletedDelegate);
DECLARE_DELEGATE_FourParams(FBattleExecutionStartedDelegate, const FBattleAction&, const FBattleExecutionEntry&, int32, const FTargetingResult&);

UCLASS()
class MUKSI_API UBattleActionExecutor : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleManager* InBattleManager, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType);
	bool ExecuteBattleAction(const FBattleAction& Action);
	void Stop();
	bool IsRunning() const { return bRunning; }

	FBattleActionCompletedDelegate OnBattleActionCompleted;
	FBattleExecutionStartedDelegate OnBattleExecutionStarted;

private:
	bool ValidateAction(const FBattleAction& Action) const;
	bool ResolveActionTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const;
	UMuksiBattleCardDataAsset* ResolveExecutionCard(const FBattleAction& Action) const;
	bool BindAttackerNotify();
	void UnbindAttackerNotify();
	bool StartMainExecutions();
	void StartNotifyExecutionChains(FName NotifyKey);
	bool RunExecutionSequence(const TArray<FBattleExecutionEntry>& ExecutionEntries);
	void HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext);
	void HandleExecutionSequenceFinished(UBattleExecutionRunner* FinishedRunner);
	void TryCompleteAction();
	void CompleteAction();
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
	FTargetingResult ActionTargetingResult;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> AttackerAnimationComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBattleExecutionRunner>> ActiveExecutionRunners;

	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
	bool bRunning = false;
};
