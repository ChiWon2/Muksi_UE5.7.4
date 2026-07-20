#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecution.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecutionContext.h"
#include "BattleSequenceManager.generated.h"

class ABattleGridManager;
class UMuksiBattleAnimationComponent;
class UMuksiBattleExecution;

DECLARE_MULTICAST_DELEGATE(FOnBattleSequenceFinished);

UCLASS()
class MUKSI_API ABattleSequenceManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSequenceManager();

public:
	FOnBattleSequenceFinished OnSequenceFinished;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Sequence")
	TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Battle|Sequence")
	bool StartSequence(const FBattleAction& InAction);

	UFUNCTION(BlueprintPure, Category = "Battle|Sequence")
	bool IsSequenceRunning() const { return bSequenceRunning; }

private:
	UPROPERTY(Transient)
	FBattleAction CurrentAction;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleAnimationComponent> AttackerAnimationComponent = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleExecution>> RunningExecutions;

	bool bSequenceRunning = false;

private:
	bool ValidateAction(const FBattleAction& InAction) const;
	bool BindAttackerNotify();
	void UnbindAttackerNotify();

	void StartInitialExecutionChain();
	void ExecuteNotifyExecutionBindings(FName NotifyKey);

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

	void ExecuteExecutionClass(TSubclassOf<UMuksiBattleExecution> ExecutionClass, FName NotifyKey);
	void ExecuteExecutionClassWithContext(TSubclassOf<UMuksiBattleExecution> ExecutionClass, const FMuksiBattleExecutionContext& Context);
	void ExecuteExecutionInstanceWithContext(UMuksiBattleExecution* Execution, const FMuksiBattleExecutionContext& Context);
	void HandleSystemExecutionRequested(TSubclassOf<UMuksiBattleExecution> ExecutionClass, const FMuksiBattleExecutionContext& Context);

	FMuksiBattleExecutionContext MakeExecutionContext(FName NotifyKey);

	void HandleExecutionFinished(UMuksiBattleExecution* FinishedExecution);
	void TryFinishSequence();
	void FinishSequence();
};