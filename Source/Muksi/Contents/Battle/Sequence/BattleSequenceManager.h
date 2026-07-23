#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleExecutionTypes.h"
#include "BattleSequenceManager.generated.h"

class ABattleGridManager;
class UBattleExecutionChain;
class UMuksiBattleAnimationComponent;

DECLARE_MULTICAST_DELEGATE(FOnBattleSequenceFinished);

UCLASS()
class MUKSI_API ABattleSequenceManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSequenceManager();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
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
	TArray<TObjectPtr<UBattleExecutionChain>> ActiveExecutionChains;

	bool bSequenceRunning = false;

private:
	bool ValidateAction(const FBattleAction& InAction) const;
	bool BindAttackerNotify();
	void UnbindAttackerNotify();

	void StartMainExecutionChain();
	void StartNotifyExecutionChains(FName NotifyKey);
	void StartExecutionChain(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);
	void HandleRuntimeExecutionChainRequested(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

	FBattleExecutionContext MakeExecutionContext(FName NotifyKey);

	void HandleExecutionChainFinished(UBattleExecutionChain* FinishedChain);
	void TryFinishSequence();
	void FinishSequence();
};