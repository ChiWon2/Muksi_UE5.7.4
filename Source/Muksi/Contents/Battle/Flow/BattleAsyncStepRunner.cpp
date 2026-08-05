#include "Muksi/Contents/Battle/Flow/BattleAsyncStepRunner.h"

bool UBattleAsyncStepRunner::Start(
    TArray<FBattleAsyncStep>&& InSteps,
    FSimpleDelegate InCompletionDelegate)
{
    if (bRunning)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("[BattleAsyncStepRunner] Ignored overlapping sequence start. Owner=%s"),
            *GetNameSafe(GetOuter()));
        return false;
    }

    ResetExecutionState(true);

    Steps = MoveTemp(InSteps);
    CompletionDelegate = MoveTemp(InCompletionDelegate);
    bRunning = true;
    ExecuteNextStep();
    return true;
}

void UBattleAsyncStepRunner::Cancel()
{
    ResetExecutionState(true);
}

void UBattleAsyncStepRunner::BeginDestroy()
{
    Cancel();
    Super::BeginDestroy();
}

void UBattleAsyncStepRunner::ExecuteNextStep()
{
    if (!bRunning || bWaitingForCurrentStep || bInvokingCurrentStep)
    {
        return;
    }

    while (bRunning && !bWaitingForCurrentStep)
    {
        ++CurrentStepIndex;
        if (!Steps.IsValidIndex(CurrentStepIndex))
        {
            FinishExecution();
            return;
        }

        const int32 StepExecutionSerial = ExecutionSerial;
        const int32 StepIndex = CurrentStepIndex;

        bWaitingForCurrentStep = true;
        const FSimpleDelegate StepFinished = FSimpleDelegate::CreateWeakLambda(
            this,
            [this, StepExecutionSerial, StepIndex]()
            {
                HandleCurrentStepFinished(StepExecutionSerial, StepIndex);
            });

        // Step이 동기적으로 취소되거나 Sequence를 완료하더라도 실행 중인
        // 함수 객체가 Steps.Reset()으로 파괴되지 않도록 로컬 복사본을 호출한다.
        const FBattleAsyncStep Step = Steps[CurrentStepIndex];

        bInvokingCurrentStep = true;
        if (Step)
        {
            Step(StepFinished);
        }
        else
        {
            StepFinished.ExecuteIfBound();
        }
        bInvokingCurrentStep = false;

        if (!bRunning)
        {
            return;
        }

        if (bAdvanceRequested)
        {
            bAdvanceRequested = false;
            continue;
        }

        return;
    }
}

void UBattleAsyncStepRunner::HandleCurrentStepFinished(
    int32 InExecutionSerial,
    int32 InStepIndex)
{
    if (!bRunning
        || !bWaitingForCurrentStep
        || InExecutionSerial != ExecutionSerial
        || InStepIndex != CurrentStepIndex)
    {
        return;
    }

    bWaitingForCurrentStep = false;

    if (bInvokingCurrentStep)
    {
        bAdvanceRequested = true;
        return;
    }

    ExecuteNextStep();
}

void UBattleAsyncStepRunner::FinishExecution()
{
    if (!bRunning)
    {
        return;
    }

    FSimpleDelegate FinishedDelegate = MoveTemp(CompletionDelegate);
    ResetExecutionState(false);
    FinishedDelegate.ExecuteIfBound();
}

void UBattleAsyncStepRunner::ResetExecutionState(bool bInvalidatePendingCallbacks)
{
    if (bInvalidatePendingCallbacks)
    {
        ++ExecutionSerial;
    }

    Steps.Reset();
    CompletionDelegate.Unbind();
    CurrentStepIndex = INDEX_NONE;
    bRunning = false;
    bWaitingForCurrentStep = false;
    bInvokingCurrentStep = false;
    bAdvanceRequested = false;
}
