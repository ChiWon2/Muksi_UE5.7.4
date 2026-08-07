#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "BattleRoundPhaseCoordinator.generated.h"

class ABattleCharacterBase;
class ABattleManager;
class UBattleAsyncStepRunner;

/**
 * RoundStart / RoundEnd에 실행할 작업의 순서만 구성한다.
 * 실제 비동기 순차 실행, 취소, 중복 완료 방지는 BattleAsyncStepRunner가 담당한다.
 */
UCLASS()
class MUKSI_API UBattleRoundPhaseCoordinator : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(ABattleManager* InBattleManager);
    void Shutdown();

protected:
    virtual void BeginDestroy() override;

private:
    UFUNCTION()
    void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

    void BeginRoundPhaseExecution(EBattlePhase NewPhase);
    void ExecutePassiveStep(
        ABattleCharacterBase* Character,
        EBattlePhase NewPhase,
        const FSimpleDelegate& CompletionDelegate) const;
    void ExecuteStatusEffectStep(
        ABattleCharacterBase* Character,
        EBattlePhase Phase,
        const FSimpleDelegate& CompletionDelegate) const;
    void FinishRoundPhaseExecution(EBattlePhase FinishedPhase);
    void CancelRoundPhaseExecution();

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleAsyncStepRunner> StepRunner = nullptr;
};
