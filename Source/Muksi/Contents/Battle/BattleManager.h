// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "BattleManager.generated.h"

class UBattleRoundPhaseCoordinator;
class UBattleRuntimeContext;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBattlePhaseChanged, EBattlePhase, OldPhase, EBattlePhase, NewPhase);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattlePhaseNativeEvent, EBattlePhase, EBattlePhase);

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
    UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
    FOnBattlePhaseChanged ChangePhaseDelegate;

    UPROPERTY(BlueprintAssignable, Category = "Battle|Event")
    FOnBattlePhaseChanged PhaseUIDelegate;

    FOnBattlePhaseNativeEvent PhaseUIFinishedDelegate;

public:
    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetCurrentRound() const { return CurrentRound; }

    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetCurrentExchange() const { return CurrentExchange; }

    UFUNCTION(BlueprintPure, Category = "Battle")
    int32 GetMaxExchangeCount() const { return MaxExchangeCount; }

    UFUNCTION(BlueprintPure, Category = "Battle|Runtime")
    UBattleRuntimeContext* GetBattleRuntimeContext() const { return BattleRuntimeContext; }

    EBattlePhase GetCurrentPhase() const { return CurrentPhase; }


    UFUNCTION(BlueprintCallable, Category = "Battle|Phase")
    void NotifyPhaseExecutionFinished();

    UFUNCTION(BlueprintCallable, Category = "Battle|Phase")
    void NotifyPhaseUIFinished(EBattlePhase FinishedPhase);

    void NotifyInteractivePhaseFinished(EBattlePhase FinishedPhase);
    void RestartCurrentExchangeCardSelection();
    void NotifyBattleCharacterDead();

    void ReadyStart();

private:
    void ChangePhase(EBattlePhase NewPhase);
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
    bool ShouldWaitForPhaseUI(EBattlePhase Phase) const;
    bool ShouldWaitForExternalExecutionAfterUI(EBattlePhase Phase) const;
    bool RequestPhaseUI();

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
    TObjectPtr<UBattleRoundPhaseCoordinator> RoundPhaseCoordinator = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    EBattlePhase CurrentPhase = EBattlePhase::None;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle")
    EBattlePhase PreviousPhase = EBattlePhase::None;

    bool bIsCharacterDead = false;

    bool bCurrentPhaseUIFinished = false;
};
