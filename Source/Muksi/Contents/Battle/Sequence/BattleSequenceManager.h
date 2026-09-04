#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "BattleSequenceManager.generated.h"

class ABattleGridManager;
class ABattleManager;
class UBattleRuntimeContext;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleActionExecutor;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeceiveCardRevealRequested, const FBattleAction&);
DECLARE_MULTICAST_DELEGATE(FOnBattleActionCompleted);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBattleExecutionEntryStarted, const FBattleAction&, const FBattleExecutionEntry&, int32, const FTargetingResult&);

UCLASS()
class MUKSI_API ABattleSequenceManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSequenceManager();
	bool InitializeBattleFlow(ABattleManager* InBattleManager, UBattleRuntimeContext* InBattleRuntimeContext, ABattleGridManager* InBattleGridManager);
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 실제 BattleActionQueue에서 변초 Action 실행 직전에 Reveal UI를 요청한다.
	FOnDeceiveCardRevealRequested DeceiveCardRevealRequestedDelegate;

	// 단일 BattleAction 완료 이벤트.
	FOnBattleActionCompleted OnBattleActionCompleted;

	FOnBattleExecutionEntryStarted OnExecutionEntryStarted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartBattleAction(const FBattleAction& InAction);

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsBattleActionRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartBattleActionSequence(const TArray<FBattleAction>& InActions);

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsBattleActionSequenceRunning() const { return bBattleActionSequenceRunning; }

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	int32 GetCurrentBattleActionIndex() const { return CurrentBattleActionIndex; }

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	void NotifyDeceiveCardRevealFinished();

	void InitializeBattleRuntimeContext(UBattleRuntimeContext* InBattleRuntimeContext);

private:
	UPROPERTY(Transient)
	TArray<FBattleAction> BattleActionQueue;

	int32 CurrentBattleActionIndex = INDEX_NONE;
	FTimerHandle NextBattleActionTimerHandle;
	bool bBattleActionSequenceRunning = false;
	bool bBattleActionCompletionPending = false;
	bool bStartingQueuedBattleAction = false;
	bool bWaitingForDeceiveCardReveal = false;
	UPROPERTY(Transient)
	TObjectPtr<UBattlePhaseTask> PhaseExecutionTask = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleRuntimeContext> BattleRuntimeContext = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UBattleActionExecutor> ActionExecutor = nullptr;


private:
	UFUNCTION()
	void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	void ExecuteBattleActionSequence();
	void NotifyBattleActionSequenceCompleted();
	void PresentBattleActionTargetingResult(const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void ClearBattleActionPresentation();

	void HandleActionExecutorEntryStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FTargetingResult& TargetingResult);
	void HandleActionExecutorFinished();

	void SortBattleActionQueue();
	void StartCurrentBattleAction();
	bool ShouldRequestDeceiveCardReveal(const FBattleAction& Action) const;
	void StartCurrentBattleActionExecution();
	void HandleCurrentBattleActionFinished();
	void FinishCurrentBattleAction();
	void StartNextBattleActionDeferred();
	void FinishBattleActionSequence();
	void ResetBattleActionSequence();
};
