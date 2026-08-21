#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "BattleSequenceManager.generated.h"

class ABattleGridManager;
class ABattleManager;
class UBattleRuntimeContext;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleExecutionRunner;
class UMuksiBattleAnimationComponent;
class UMuksiBattleCardDataAsset;
class UBattleSequenceExecutionEnvironment;
class UTargetingPresentationController;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDeceiveCardRevealRequested, const FBattleAction&);
DECLARE_MULTICAST_DELEGATE(FOnBattleSequenceFinished);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBattleExecutionEntryStarted, const FBattleAction&, const FBattleExecutionEntry&, int32, const FResolvedTargeting&);

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

	// 단일 Action의 Execution Chain 완료 이벤트. Simulation에서도 기존대로 사용한다.
	FOnBattleSequenceFinished OnSequenceFinished;

	FOnBattleExecutionEntryStarted OnExecutionEntryStarted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartSequence(const FBattleAction& InAction);

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartSequenceWithRequest(const FBattleSequenceRequest& Request);

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsSequenceRunning() const { return bSequenceRunning; }

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

	bool bSequenceRunning = false;
	EBattleExecutionMode CurrentExecutionMode = EBattleExecutionMode::Sequence;

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
	TObjectPtr<UTargetingPresentationController> TargetingPresentationController = nullptr;

private:
	UFUNCTION()
	void HandlePhaseExecutionRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

	void ExecuteBattleActionSequence();
	void NotifyBattleActionSequenceCompleted();
	void RefreshBattleActionTargetingPresentation(const FBattleAction& Action, const FResolvedTargeting& ExecutionResolvedTargeting);
	void ClearBattleActionPresentation();

	bool ValidateRequest(const FBattleSequenceRequest& Request) const;
	bool InitializeExecutionEnvironment();
	bool BindAttackerNotify();
	void UnbindAttackerNotify();

	void StartMainExecutionChain();
	void StartNotifyExecutionChains(FName NotifyKey);
	void StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

	FBattleExecutionContext MakeExecutionContext(FName NotifyKey);

	void HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext);
	void HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
	void TryFinishSequence();
	void FinishSequence();

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
