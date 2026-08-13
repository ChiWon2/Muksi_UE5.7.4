#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "BattleCharacterPhaseCoordinator.generated.h"

class ABattleCharacterBase;
class ABattleManager;
class UBattleAsyncStepRunner;

/**
 * 캐릭터의 Passive와 StatusEffect를 정해진 순서로 실행한다.
 * Phase UI가 완료된 뒤 Player Passive, Enemy Passive, Player StatusEffect, Enemy StatusEffect 순서로 실행한다.
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
    void HandlePhaseUIFinished(EBattlePhase OldPhase, EBattlePhase NewPhase);

    bool RequiresSequentialExecution(EBattlePhase Phase) const;
    bool RequiresManualCompletion(EBattlePhase Phase) const;
    void BeginCharacterPhaseExecution(EBattlePhase OldPhase, EBattlePhase NewPhase);
    void ExecutePassiveStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const;
    void ExecuteStatusEffectStep(ABattleCharacterBase* Character, EBattlePhase OldPhase, EBattlePhase NewPhase, const FSimpleDelegate& CompletionDelegate) const;
    void FinishCharacterPhaseExecution(EBattlePhase FinishedPhase);
    void NotifyCharacterPhaseFinishedIfCurrent(EBattlePhase FinishedPhase);
    void CancelCharacterPhaseExecution();

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleAsyncStepRunner> StepRunner = nullptr;

    EBattlePhase ExecutingPhase = EBattlePhase::None;
};
