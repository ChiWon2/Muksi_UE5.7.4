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

DECLARE_MULTICAST_DELEGATE_OneParam(FDeceiveCardRevealRequestedDelegate, const FBattleAction&);
DECLARE_MULTICAST_DELEGATE_OneParam(FBattleSequenceActionStartedDelegate, const FBattleAction&);
DECLARE_MULTICAST_DELEGATE_OneParam(FBattleSequenceActionCompletedDelegate, const FBattleAction&);

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
	FDeceiveCardRevealRequestedDelegate DeceiveCardRevealRequestedDelegate;

	// 실제 AA BattleAction 실행 시작 이벤트.
	FBattleSequenceActionStartedDelegate BattleActionStartedDelegate;

	// 단일 BattleAction 완료 이벤트.
	FBattleSequenceActionCompletedDelegate BattleActionCompletedDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;
	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsBattleActionRunning() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartBattleActionSequence(const TArray<FBattleAction>& InBattleActions);

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsBattleActionSequenceRunning() const { return bBattleActionSequenceRunning; }

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	int32 GetCurrentBattleActionIndex() const { return CurrentBattleActionIndex; }

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	void NotifyDeceiveCardRevealFinished();
	void StopAfterCurrentExecution();

	void InitializeBattleRuntimeContext(UBattleRuntimeContext* InBattleRuntimeContext);

private:
	UPROPERTY(Transient)
	TArray<FBattleAction> BattleActionQueue;

	int32 CurrentBattleActionIndex = INDEX_NONE;
	FTimerHandle NextBattleActionTimerHandle;
	bool bBattleActionSequenceRunning = false;
	bool bBattleActionCompletionPending = false;
	bool bWaitingForDeceiveCardReveal = false;
	bool bStopAfterCurrentExecution = false;
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
	void CompleteBattleActionSequencePhase();
	void PresentBattleActionTargetingResult(const FBattleAction& Action, const FTargetingResult& TargetingResult);
	void ClearBattleActionPresentation();

	void HandleBattleActionStarted(const FBattleAction& Action);
	void HandleExecutionEntryStarted(const FBattleAction& Action, const FBattleExecutionEntry& Entry, int32 EntryIndex, const FTargetingResult& TargetingResult);
	void HandleBattleActionCompleted();

	void SortBattleActionQueue();
	void StartCurrentBattleAction();
	bool ShouldRequestDeceiveCardReveal(const FBattleAction& Action) const;
	void ExecuteCurrentBattleAction();
	void FinishCurrentBattleAction();
	void StartNextBattleActionDeferred();
	void FinishBattleActionSequence();
	void ResetBattleActionSequence();
};
