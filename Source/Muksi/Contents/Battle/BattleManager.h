// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Flow/BattleFlowDelegates.h"
#include "BattleManager.generated.h"

class UBattlePhasePipeline;
class UBattlePhaseTaskContext;
class UBattleRuntimeContext;
class ABattleGridManager;
class ABattleSequenceManager;
class ABattleSetupManager;
class ABattleSimulationManager;
class ABattleTargetingManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnBattlePhaseStageRequested, EBattlePhase, OldPhase, EBattlePhase, NewPhase, UBattlePhaseTaskContext*, TaskContext);

UCLASS()
class MUKSI_API ABattleManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleManager();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
    UPROPERTY(BlueprintAssignable, Category = "Battle|Phase")
    FOnBattlePhaseStageRequested PhaseEntryRequestedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Battle|Phase")
    FOnBattlePhaseStageRequested PhaseUIRequestedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Battle|Phase")
    FOnBattlePhaseStageRequested PhasePrepRequestedDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Battle|Phase")
    FOnBattlePhaseStageRequested PhaseExecutionRequestedDelegate;

    FOnBattleActionStart BattleActionStartDelegate;

public:
    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetCurrentRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetCurrentExchange() const { return CurrentExchange; }

    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetMaxExchangeCount() const { return MaxExchangeCount; }

    UFUNCTION(BlueprintPure, Category = "Battle|Runtime")
    UBattleRuntimeContext* GetBattleRuntimeContext() const { return BattleRuntimeContext; }

    UFUNCTION(BlueprintPure, Category = "Battle|Managers")
    ABattleGridManager* GetBattleGridManager() const { return BattleGridManager; }

    UFUNCTION(BlueprintPure, Category = "Battle|Managers")
    ABattleSetupManager* GetBattleSetupManager() const { return BattleSetupManager; }

    UFUNCTION(BlueprintPure, Category = "Battle|Managers")
    ABattleTargetingManager* GetBattleTargetingManager() const { return BattleTargetingManager; }

    UFUNCTION(BlueprintPure, Category = "Battle|Managers")
    ABattleSimulationManager* GetBattleSimulationManager() const { return BattleSimulationManager; }

    UFUNCTION(BlueprintPure, Category = "Battle|Managers")
    ABattleSequenceManager* GetBattleSequenceManager() const { return BattleSequenceManager; }

    EBattlePhase GetCurrentPhase() const { return CurrentPhase; }

    void RestartCurrentExchangeCardSelection();
    void NotifyBattleCharacterDead();
    void NotifyBattleActionStart(const FBattleAction& BattleAction);
    void StartBattleFlow();

private:
    bool InitializeBattleFlow();
    void ChangePhase(EBattlePhase NewPhase);

    void ExecutePhaseEntry();    
    void HandlePhaseEntryFinished(EBattlePhase FinishedPhase);

    void ExecutePhaseUI();
    void HandlePhaseUIFinished(EBattlePhase FinishedPhase);

    void ExecutePhasePrep();
    void HandlePhasePrepFinished(EBattlePhase FinishedPhase);
    
    void ExecutePhaseExecution();
    void HandlePhaseExecutionFinished(EBattlePhase FinishedPhase);
    
    bool IsCurrentPhaseCompletion(EBattlePhase FinishedPhase, const TCHAR* StageName) const;

    void ReadyStart();
    void ReadyEnd();
    void BattleStart();
    void BattleEnd();
    void RoundStart();
    void RoundEnd();
    void ExchangeStart();
    void StartExchangeSelectCard();
    void ExchangeEnd();
    void BattleActionSequenceStart();
    void BattleActionSequenceEnd();
    bool ShouldEndBattle() const;
    void EndBattleLevel();

    void AdvanceExchange();
    void AdvanceFromPhase(EBattlePhase FinishedPhase);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    int32 CurrentRound = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    int32 CurrentExchange = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Rule")
    int32 MaxExchangeCount = 3;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Battle|Runtime")
    TObjectPtr<UBattleRuntimeContext> BattleRuntimeContext = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhasePipeline> PhasePipeline = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle|Managers", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABattleSetupManager> BattleSetupManager = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle|Managers", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle|Managers", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABattleTargetingManager> BattleTargetingManager = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle|Managers", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABattleSimulationManager> BattleSimulationManager = nullptr;

    UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Battle|Managers", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ABattleSequenceManager> BattleSequenceManager = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    EBattlePhase CurrentPhase = EBattlePhase::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    EBattlePhase PreviousPhase = EBattlePhase::None;

    bool bIsCharacterDead = false;
    bool bBattleFlowInitialized = false;
    bool bBattleFlowStarted = false;
};
