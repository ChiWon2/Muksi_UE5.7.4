#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionContext.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Sequence/Data/BattleSequenceRequest.h"
#include "BattleSequenceManager.generated.h"

class ABattleGridManager;
class UBattleExecutionRunner;
class UMuksiBattleAnimationComponent;
class UBattleSequenceExecutionEnvironment;

DECLARE_MULTICAST_DELEGATE(FOnBattleSequenceFinished);
DECLARE_MULTICAST_DELEGATE_FourParams(FOnBattleExecutionEntryStarted, const FBattleAction&, const FBattleExecutionEntry&, int32, const FResolvedTargeting&);

UCLASS()
class MUKSI_API ABattleSequenceManager : public AActor
{
	GENERATED_BODY()

public:
	ABattleSequenceManager();
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool bWorldManagerRegistrationEnabled = true;
public:
	void SetWorldManagerRegistrationEnabled(bool bEnabled) { bWorldManagerRegistrationEnabled = bEnabled; }

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

private:
	UPROPERTY(Transient)
	FBattleAction CurrentAction;

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

private:
	bool ValidateRequest(const FBattleSequenceRequest& Request) const;
	bool InitializeExecutionEnvironment();
	bool BindAttackerNotify();
	void UnbindAttackerNotify();

	void StartMainExecutionChain();
	void StartNotifyExecutionChains(FName NotifyKey);
	void StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);
	void HandleRuntimeExecutionChainRequested(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context);

	UFUNCTION()
	void HandleBattleExecutionNotify(FName NotifyKey);

	FBattleExecutionContext MakeExecutionContext(FName NotifyKey);

	void HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext);
	void HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner);
	void TryFinishSequence();
	void FinishSequence();
};
