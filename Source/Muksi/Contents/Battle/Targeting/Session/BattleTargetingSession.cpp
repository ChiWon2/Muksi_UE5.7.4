#include "Muksi/Contents/Battle/Targeting/Session/BattleTargetingSession.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Hex/HexGridMath.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Selection/TargetSelection.h"
#include "Muksi/Contents/Battle/Targeting/Types/TargetingOriginSource.h"

bool UBattleTargetingSession::StartSession(ABattleCharacterBase* InSourceCharacter, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType, const FTargetingCardData& InCardTargetingData)
{
    ResetSession();

    if (!IsValid(InSourceCharacter) || !IsValid(InGridManager))
        return false;

    SourceCharacter = InSourceCharacter;
    GridManager = InGridManager;
    GridWorldType = InGridWorldType;
    CardTargetingData = InCardTargetingData;

    if (!CardTargetingData.HasSteps())
    {
        State = EBattleTargetingSessionState::Completed;
        return true;
    }

    CurrentStepIndex = 0;
    State = EBattleTargetingSessionState::Selecting;
    return true;
}

bool UBattleTargetingSession::UpdateSelection(const FHexOffsetCoord& CandidateCoord, int32 Direction)
{
    if (!IsSelecting() || !CandidateCoord.IsValid())
    {
        ResetCurrentStep();
        return false;
    }

    FTargetingStep Step;
    if (!EvaluateCandidate(CandidateCoord, Step))
    {
        ResetCurrentStep();
        return false;
    }

    Step.Direction = Direction;

    const FTargetingStepCardData* StepData = GetCurrentStepData();
    if (!Step.HasDirection()
        && StepData
        && StepData->Origin.Source == ETargetingOriginSource::PreviousStep
        && !ConfirmedSteps.IsEmpty()
        && ConfirmedSteps.Last().Step.HasDirection())
    {
        Step.Direction = ConfirmedSteps.Last().Step.Direction;
    }

    if (StepData && StepData->Pattern.PatternClass)
    {
        const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
        if (Pattern && Pattern->RequiresDirection() && !Step.HasDirection())
        {
            ResetCurrentStep();
            return false;
        }
    }

    FTargetingStepResult TargetingStepResult;
    if (!BuildTargetingStepResult(Step, TargetingStepResult))
    {
        ResetCurrentStep();
        return false;
    }

    CurrentStepResult = MoveTemp(TargetingStepResult);
    return true;
}

ETargetingConfirmResult UBattleTargetingSession::ConfirmStep()
{
    if (!IsSelecting() || !CardTargetingData.IsValidStepIndex(CurrentStepIndex) || !CurrentStepResult.Step.HasOriginCoord())
        return ETargetingConfirmResult::Failed;

    ConfirmedSteps.Add(CurrentStepResult);

    const int32 NextStepIndex = CurrentStepIndex + 1;

    if (CardTargetingData.IsValidStepIndex(NextStepIndex))
    {
        CurrentStepIndex = NextStepIndex;
        ResetCurrentStep();
        return ETargetingConfirmResult::AdvancedToNextStep;
    }

    if (!BuildIntent())
    {
        ConfirmedSteps.Pop();
        return ETargetingConfirmResult::Failed;
    }

    State = EBattleTargetingSessionState::Completed;
    return ETargetingConfirmResult::Completed;
}

bool UBattleTargetingSession::UndoStep()
{
    if (ConfirmedSteps.IsEmpty())
        return false;

    const int32 RestoredStepIndex = ConfirmedSteps.Num() - 1;

    if (!CardTargetingData.IsValidStepIndex(RestoredStepIndex))
        return false;

    CurrentStepResult = ConfirmedSteps.Pop();
    CurrentStepIndex = RestoredStepIndex;
    State = EBattleTargetingSessionState::Selecting;
    Intent.Reset();
    return true;
}

bool UBattleTargetingSession::IsSelecting() const
{
    return State == EBattleTargetingSessionState::Selecting;
}

bool UBattleTargetingSession::IsCompleted() const
{
    return State == EBattleTargetingSessionState::Completed;
}

int32 UBattleTargetingSession::GetCurrentStepIndex() const
{
    return CurrentStepIndex;
}

const FTargetingIntent& UBattleTargetingSession::GetIntent() const
{
    return Intent;
}

const FTargetingStepResult& UBattleTargetingSession::GetCurrentStepResult() const
{
    return CurrentStepResult;
}

const TArray<FTargetingStepResult>& UBattleTargetingSession::GetConfirmedSteps() const
{
    return ConfirmedSteps;
}

const FTargetingCardData& UBattleTargetingSession::GetCardTargetingData() const
{
    return CardTargetingData;
}

ABattleCharacterBase* UBattleTargetingSession::GetSourceCharacter() const
{
    return SourceCharacter.Get();
}

const FTargetingStepCardData* UBattleTargetingSession::GetCurrentStepData() const
{
    return CardTargetingData.GetStep(CurrentStepIndex);
}

bool UBattleTargetingSession::GetCurrentOriginCoord(FHexOffsetCoord& OutOriginCoord) const
{
    OutOriginCoord = FHexOffsetCoord::Invalid();

    const FTargetingStepCardData* StepData = GetCurrentStepData();

    if (!StepData)
        return false;

    switch (StepData->Origin.Source)
    {
    case ETargetingOriginSource::SourceCharacter:
        if (!IsValid(SourceCharacter.Get()))
            return false;

        OutOriginCoord = SourceCharacter->GetCharacterCoord();
        break;

    case ETargetingOriginSource::PreviousStep:
        if (ConfirmedSteps.IsEmpty() || !ConfirmedSteps.Last().Step.HasTargetCoord())
            return false;

        OutOriginCoord = ConfirmedSteps.Last().Step.TargetCoord;
        break;

    case ETargetingOriginSource::SpecificStep:
        if (!ConfirmedSteps.IsValidIndex(StepData->Origin.StepIndex) || !ConfirmedSteps[StepData->Origin.StepIndex].Step.HasTargetCoord())
            return false;

        OutOriginCoord = ConfirmedSteps[StepData->Origin.StepIndex].Step.TargetCoord;
        break;

    default:
        return false;
    }

    return IsValid(GridManager.Get()) && GridManager->IsValidCoord(OutOriginCoord);
}

bool UBattleTargetingSession::EvaluateCandidate(const FHexOffsetCoord& CandidateCoord, FTargetingStep& OutStep) const
{
    OutStep.Reset();

    const FTargetingStepCardData* StepData = GetCurrentStepData();

    if (!StepData || !StepData->Selection.RuleClass)
        return false;

    const UTargetSelection* Selection = StepData->Selection.RuleClass->GetDefaultObject<UTargetSelection>();

    if (!Selection)
        return false;

    FHexOffsetCoord OriginCoord;

    if (!GetCurrentOriginCoord(OriginCoord))
        return false;

    return Selection->EvaluateCandidate(GridManager.Get(), OriginCoord, CandidateCoord, StepData->Selection.RuleData, OutStep);
}

bool UBattleTargetingSession::BuildTargetingStepResult(const FTargetingStep& Step, FTargetingStepResult& OutStepResult) const
{
    OutStepResult.Reset();

    if (!Step.HasOriginCoord() || !Step.HasTargetCoord())
        return false;

    const FTargetingStepCardData* StepData = GetCurrentStepData();
    if (!StepData || !IsValid(GridManager.Get()))
        return false;

    OutStepResult.Step = Step;

    if (!StepData->Pattern.PatternClass)
    {
        OutStepResult.AffectedCoords.Add(Step.TargetCoord);
    }
    else
    {
        const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
        if (!Pattern)
            return false;

        Pattern->ApplyPattern(
            GridManager.Get(),
            GridWorldType,
            StepData->Pattern.PatternData,
            Step.OriginCoord,
            Step.TargetCoord,
            Step.Direction,
            OutStepResult.AffectedCoords,
            OutStepResult.PathCoords);
    }

    GridManager->GetCharactersAtCoords(GridWorldType, OutStepResult.AffectedCoords, OutStepResult.Targets);
    return true;
}

bool UBattleTargetingSession::BuildIntent()
{
    Intent.Reset();

    if (ConfirmedSteps.Num() != CardTargetingData.Steps.Num())
        return false;

    Intent.Steps.Reserve(ConfirmedSteps.Num());

    for (int32 StepIndex = 0; StepIndex < ConfirmedSteps.Num(); ++StepIndex)
    {
        const FTargetingStepCardData* StepData = CardTargetingData.GetStep(StepIndex);
        if (!StepData)
        {
            Intent.Reset();
            return false;
        }

        FTargetingStepIntent StepIntent;
        if (!BuildStepIntent(ConfirmedSteps[StepIndex], *StepData, StepIntent))
        {
            Intent.Reset();
            return false;
        }

        Intent.Steps.Add(MoveTemp(StepIntent));
    }

    return true;
}

bool UBattleTargetingSession::BuildStepIntent(const FTargetingStepResult& StepResult, const FTargetingStepCardData& StepData, FTargetingStepIntent& OutIntent) const
{
    OutIntent.Reset();

    const FTargetingStep& Step = StepResult.Step;
    if (!Step.HasOriginCoord() || !Step.HasTargetCoord())
        return false;

    OutIntent.Direction = Step.Direction;

    switch (StepData.Intent.Binding)
    {
    case ETargetingIntentBinding::TargetCharacter:
        OutIntent.RelativeOffset = FHexGridMath::OffsetToCube(Step.TargetCoord) - FHexGridMath::OffsetToCube(Step.OriginCoord);

        if (const FBattleGridCell* Cell = GridManager->GetCellByCoord(GridWorldType, Step.TargetCoord))
        {
            if (const ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()))
                OutIntent.TargetCharacterKey = TargetCharacter->GetTargetingCharacterKey();
        }

        return OutIntent.HasTargetCharacterKey();

    case ETargetingIntentBinding::WorldFixed:
        OutIntent.SelectedCoord = Step.TargetCoord;
        return OutIntent.SelectedCoord.IsValid();

    case ETargetingIntentBinding::OriginRelative:
    default:
        OutIntent.RelativeOffset = FHexGridMath::OffsetToCube(Step.TargetCoord) - FHexGridMath::OffsetToCube(Step.OriginCoord);
        return true;
    }
}

void UBattleTargetingSession::ResetCurrentStep()
{
    CurrentStepResult.Reset();
}

void UBattleTargetingSession::ResetSession()
{
    SourceCharacter = nullptr;
    GridManager = nullptr;
    GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;
    CardTargetingData = FTargetingCardData();
    CurrentStepIndex = INDEX_NONE;
    CurrentStepResult.Reset();
    ConfirmedSteps.Empty();
    Intent.Reset();
    State = EBattleTargetingSessionState::Idle;
}
