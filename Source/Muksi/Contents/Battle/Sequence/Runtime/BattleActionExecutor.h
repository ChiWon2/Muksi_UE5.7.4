#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleActionExecutor.generated.h"

class ABattleGridManager;
class UBattleExecutionRunner;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;

DECLARE_DELEGATE_OneParam(FBattleActionStartedDelegate, const FBattleAction&);
DECLARE_DELEGATE(FBattleActionCompletedDelegate);
DECLARE_DELEGATE_FourParams(FBattleActionExecutionEntryStartedDelegate, const FBattleAction&, const FBattleExecutionEntry&, int32, const FTargetingResult&);

UCLASS()
class MUKSI_API UBattleActionExecutor : public UObject
{
	GENERATED_BODY()

public:
	bool Initialize(ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType);
	bool ExecuteBattleAction(const FBattleAction& Action);
	void Stop();
	bool IsRunning() const { return bRunning; }

	FBattleActionStartedDelegate OnBattleActionStarted;
	FBattleActionCompletedDelegate OnBattleActionCompleted;
	FBattleActionExecutionEntryStartedDelegate OnExecutionEntryStarted;

private:
	bool ValidateAction(const FBattleAction& Action) const;
	bool ResolveActionTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const;
	UMuksiBattleCardDataAsset* ResolveExecutionCard(const FBattleAction& Action) const;
	bool BindAttackerNotify();
	void UnbindAttackerNotify();
	bool RunMainExecutionEntries();
	void RunExecutionEntriesForNotify(FName NotifyKey);
	bool RunExecutionEntries(const TArray<FBattleExecutionEntry>& ExecutionEntries);
	void HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext);
	void HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
	void TryCompleteAction();
	void CompleteAction();
	void ResetRuntime();

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

private:
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
