#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleAsyncStepRunner.generated.h"

using FBattleAsyncStep = TFunction<void(const FSimpleDelegate&)>;

/**
 * 완료 Delegate 기반의 작업을 한 번에 하나씩 실행하는 공통 비동기 순차 실행기다.
 * 실행 취소 이후 도착한 완료 통지와 동일 Step의 중복 완료 통지는 무시한다.
 */
UCLASS()
class MUKSI_API UBattleAsyncStepRunner : public UObject
{
    GENERATED_BODY()

public:
    bool Start(TArray<FBattleAsyncStep>&& InSteps, FSimpleDelegate InCompletionDelegate);
    void Cancel();

    bool IsRunning() const
    {
        return bRunning;
    }

    int32 GetCurrentStepIndex() const
    {
        return CurrentStepIndex;
    }

protected:
    virtual void BeginDestroy() override;

private:
    void ExecuteNextStep();
    void HandleCurrentStepFinished(int32 InExecutionSerial, int32 InStepIndex);
    void FinishExecution();
    void ResetExecutionState(bool bInvalidatePendingCallbacks);

private:
    TArray<FBattleAsyncStep> Steps;
    FSimpleDelegate CompletionDelegate;

    int32 CurrentStepIndex = INDEX_NONE;
    int32 ExecutionSerial = 0;
    bool bRunning = false;
    bool bWaitingForCurrentStep = false;
    bool bInvokingCurrentStep = false;
    bool bAdvanceRequested = false;
};
