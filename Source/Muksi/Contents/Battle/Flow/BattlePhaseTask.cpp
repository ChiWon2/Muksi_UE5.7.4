#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"

#include "Muksi/Contents/Battle/Flow/BattlePhasePipeline.h"

void UBattlePhaseTask::Initialize(UBattlePhaseTaskContext* InContext, int32 InTaskId)
{
    Context = InContext;
    TaskId = InTaskId;
    bCompleted = false;
}

void UBattlePhaseTask::Complete()
{
    if (bCompleted) 
        return;
    
    bCompleted = true;
    
    UBattlePhaseTaskContext* TaskContext = Context;
    Context = nullptr;
    
    const int32 CompletedTaskId = TaskId;
    TaskId = INDEX_NONE;

    if (TaskContext) 
        TaskContext->CompleteTask(CompletedTaskId);
}

void UBattlePhaseTask::Invalidate()
{
    bCompleted = true;
    Context = nullptr;
    TaskId = INDEX_NONE;
}

void UBattlePhaseTaskContext::Initialize(UBattlePhasePipeline* InPipeline, EBattlePhase InPhase, int32 InExecutionSerial)
{
    Pipeline = InPipeline;
    Phase = InPhase;
    ExecutionSerial = InExecutionSerial;
    NextTaskId = 0;
    bRegistrationSealed = false;
    bFinished = false;
    PendingTaskIds.Reset();
    Tasks.Reset();
}

UBattlePhaseTask* UBattlePhaseTaskContext::RegisterTask(UObject* TaskOwner)
{
    if (bRegistrationSealed || bFinished || !IsValid(TaskOwner)) 
        return nullptr;

    UBattlePhaseTask* Task = NewObject<UBattlePhaseTask>(this);
    if (!Task) 
        return nullptr;

    const int32 TaskId = NextTaskId++;
    Task->Initialize(this, TaskId);
    Tasks.Add(Task);
    PendingTaskIds.Add(TaskId);
    return Task;
}

void UBattlePhaseTaskContext::SealRegistration()
{
    if (bFinished) return;
    bRegistrationSealed = true;
    TryFinish();
}

void UBattlePhaseTaskContext::CompleteTask(int32 TaskId)
{
    if (bFinished || !PendingTaskIds.Remove(TaskId)) 
        return;
    TryFinish();
}

void UBattlePhaseTaskContext::TryFinish()
{
    if (bFinished || !bRegistrationSealed || !PendingTaskIds.IsEmpty()) 
        return;
    bFinished = true;
    UBattlePhasePipeline* OwningPipeline = Pipeline;
    Pipeline = nullptr;
    Tasks.Reset();
    if (OwningPipeline) 
        OwningPipeline->HandleTaskContextFinished(this, ExecutionSerial);
}

void UBattlePhaseTaskContext::Cancel()
{
    if (bFinished) 
        return;
    bFinished = true;
    for (UBattlePhaseTask* Task : Tasks)
    {
        if (Task) Task->Invalidate();
    }
    Tasks.Reset();
    PendingTaskIds.Reset();
    Pipeline = nullptr;
}
