#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingConfirmResult.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"

#include "BattleTargetingSession.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UTargetSelection;

UENUM(BlueprintType)
enum class EBattleTargetingSessionState : uint8
{
    Idle,
    Selecting,
    Completed
};

UCLASS(BlueprintType)
class MUKSI_API UBattleTargetingSession : public UObject
{
    GENERATED_BODY()

public:
    bool StartSession(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType, const FTargetingCardData& InCardTargetingData);
    bool UpdateSelection(const FHexOffsetCoord& CandidateCoord, int32 Direction);
    ETargetingConfirmResult ConfirmStep();
    bool UndoStep();

    bool IsSelecting() const;
    bool IsCompleted() const;
    int32 GetCurrentStepIndex() const;
    const FTargetingIntent& GetIntent() const;
    const FTargetingStepResult& GetCurrentStepResult() const;
    const TArray<FTargetingStepResult>& GetConfirmedSteps() const;
    const FTargetingCardData& GetCardTargetingData() const;
    ABattleCharacterBase* GetSourceCharacter() const;
    const FTargetingStepCardData* GetCurrentStepData() const;
    bool GetCurrentOriginCoord(FHexOffsetCoord& OutOriginCoord) const;

private:
    bool EvaluateCandidate(const FHexOffsetCoord& CandidateCoord, FTargetingStep& OutStep) const;
    bool BuildTargetingStepResult(const FTargetingStep& Step, FTargetingStepResult& OutStepResult) const;
    bool BuildIntent();
    bool BuildStepIntent(const FTargetingStepResult& StepResult, const FTargetingStepCardData& StepData, FTargetingStepIntent& OutIntent) const;
    void ResetCurrentStep();
    void ResetSession();

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ABattleGridManager> GridManager = nullptr;

    EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
    FTargetingCardData CardTargetingData;
    int32 CurrentStepIndex = INDEX_NONE;
    FTargetingStepResult CurrentStepResult;
    TArray<FTargetingStepResult> ConfirmedSteps;
    FTargetingIntent Intent;
    EBattleTargetingSessionState State = EBattleTargetingSessionState::Idle;
};
