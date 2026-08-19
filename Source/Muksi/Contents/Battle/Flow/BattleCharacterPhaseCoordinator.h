#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "BattleCharacterPhaseCoordinator.generated.h"

class ABattleCharacterBase;
class ABattleManager;
class UBattleAsyncStepRunner;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;

/**
 * 캐릭터의 Passive와 StatusEffect를 정해진 순서로 실행한다.
 * Phase Prep 요청을 받으면 Player Passive, Enemy Passive, Player StatusEffect, Enemy StatusEffect 순서로 실행한다.
 */
UCLASS()
class MUKSI_API UBattleCharacterPhaseCoordinator : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(ABattleManager* InBattleManager);
    void Shutdown();

protected:
    virtual void BeginDestroy() override;

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTask> PhasePrepTask = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleAsyncStepRunner> StepRunner = nullptr;

private:
    bool HandlesPhasePrep(EBattlePhase Phase) const;
    UFUNCTION()
    void HandlePhasePrepRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    void BeginCharacterPhaseExecution(EBattlePhase OldPhase, EBattlePhase NewPhase);
    void ExecutePassiveStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const;
    void ExecuteStatusEffectStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const;
    void FinishCharacterPhaseExecution(EBattlePhase FinishedPhase);
    void CancelCharacterPhaseExecution();

};
