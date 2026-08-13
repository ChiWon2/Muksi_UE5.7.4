#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "BattlePhaseTask.generated.h"

class UBattlePhasePipeline;
class UBattlePhaseTaskContext;

UCLASS(BlueprintType)
class MUKSI_API UBattlePhaseTask : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Battle|Phase")
    void Complete();

    UFUNCTION(BlueprintPure, Category = "Battle|Phase")
    bool IsCompleted() const { return bCompleted; }

private:
    friend class UBattlePhaseTaskContext;

    void Initialize(UBattlePhaseTaskContext* InContext, int32 InTaskId);
    void Invalidate();

private:
    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTaskContext> Context = nullptr;

    int32 TaskId = INDEX_NONE;
    bool bCompleted = false;
};



UCLASS(BlueprintType)
class MUKSI_API UBattlePhaseTaskContext : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Battle|Phase")
    UBattlePhaseTask* RegisterTask(UObject* TaskOwner);

    UFUNCTION(BlueprintPure, Category = "Battle|Phase")
    EBattlePhase GetPhase() const { return Phase; }

private:
    friend class UBattlePhasePipeline;
    friend class UBattlePhaseTask;

    void Initialize(UBattlePhasePipeline* InPipeline, EBattlePhase InPhase, int32 InExecutionSerial);
    void SealRegistration();
    void CompleteTask(int32 TaskId);
    void TryFinish();
    void Cancel();

private:
    UPROPERTY(Transient)
    TObjectPtr<UBattlePhasePipeline> Pipeline = nullptr;

    UPROPERTY(Transient)
    TArray<TObjectPtr<UBattlePhaseTask>> Tasks;

    TSet<int32> PendingTaskIds;
    EBattlePhase Phase = EBattlePhase::None;
    int32 ExecutionSerial = 0;
    int32 NextTaskId = 0;
    bool bRegistrationSealed = false;
    bool bFinished = false;
};
