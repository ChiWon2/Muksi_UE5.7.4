#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "BattlePhasePipeline.generated.h"

class ABattleManager;
class UBattleCharacterPhaseCoordinator;
class UBattlePhaseTaskContext;

UCLASS()
class MUKSI_API UBattlePhasePipeline : public UObject
{
    GENERATED_BODY()

public:
    bool Initialize(ABattleManager* InBattleManager);
    void Shutdown();
    void ExecutePhaseEntry(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);
    void ExecutePhaseUI(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);
    void ExecutePhasePrep(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);
    void ExecutePhaseExecution(EBattlePhase OldPhase, EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate);
    void HandleTaskContextFinished(UBattlePhaseTaskContext* FinishedContext, int32 FinishedSerial);

protected:
    virtual void BeginDestroy() override;

private:
    UBattlePhaseTaskContext* BeginStage(EBattlePhase Phase, FSimpleDelegate CompletionDelegate);
    void SealStage(UBattlePhaseTaskContext* StageContext);
    void CancelActiveStage();

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleCharacterPhaseCoordinator> CharacterPhaseCoordinator = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattlePhaseTaskContext> ActiveTaskContext = nullptr;

    FSimpleDelegate ActiveStageCompletionDelegate;
    int32 StageExecutionSerial = 0;
};
