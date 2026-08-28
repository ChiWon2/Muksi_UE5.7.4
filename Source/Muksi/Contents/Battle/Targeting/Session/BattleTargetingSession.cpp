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
    ResetCurrentStep();
    return true;
}

bool UBattleTargetingSession::UpdateSelection(const FHexOffsetCoord& CandidateCoord, int32 Direction)
{
    if (!IsSelecting() || !CandidateCoord.IsValid())
    {
        ResetCurrentStep();
        return false;
    }

    FSelectionStepResult SelectionResult;

    if (!EvaluateCandidate(CandidateCoord, SelectionResult))
    {
        ResetCurrentStep();
        return false;
    }

    SelectionResult.SelectedDirection = Direction;

    const FTargetingStepCardData* StepData = GetCurrentStepData();

    if (!SelectionResult.HasSelectedDirection()&& StepData && StepData->Origin.Source == ETargetingOriginSource::PreviousStep && !ConfirmedSteps.IsEmpty() && ConfirmedSteps.Last().HasSelectedDirection())
    {
        SelectionResult.SelectedDirection = ConfirmedSteps.Last().SelectedDirection;
    }

    if (StepData && StepData->Pattern.PatternClass)
    {
        const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();

        if (Pattern && Pattern->RequiresDirection() && !SelectionResult.HasSelectedDirection())
        {
            ResetCurrentStep();
            return false;
        }
    }

    CurrentStepResult = MoveTemp(SelectionResult);
    return true;
}

ETargetingConfirmResult UBattleTargetingSession::ConfirmStep()
{
    if (!IsSelecting() || !CardTargetingData.IsValidStepIndex(CurrentStepIndex) || !CurrentStepResult.bValid)
        return ETargetingConfirmResult::Failed;

    FTargetingStepIntent StepIntent;

    if (!BuildCurrentStepIntent(StepIntent))
        return ETargetingConfirmResult::Failed;

    ConfirmedSteps.Add(CurrentStepResult);
    Intent.Steps.Add(StepIntent);

    const int32 NextStepIndex = CurrentStepIndex + 1;

    if (CardTargetingData.IsValidStepIndex(NextStepIndex))
    {
        CurrentStepIndex = NextStepIndex;
        ResetCurrentStep();
        return ETargetingConfirmResult::AdvancedToNextStep;
    }

    State = EBattleTargetingSessionState::Completed;
    return ETargetingConfirmResult::Completed;
}

bool UBattleTargetingSession::UndoStep()
{
    if (ConfirmedSteps.IsEmpty() || Intent.Steps.IsEmpty())
        return false;

    const int32 RestoredStepIndex = ConfirmedSteps.Num() - 1;

    if (!CardTargetingData.IsValidStepIndex(RestoredStepIndex))
        return false;

    CurrentStepResult = ConfirmedSteps.Pop();
    Intent.Steps.Pop();
    CurrentStepIndex = RestoredStepIndex;
    State = EBattleTargetingSessionState::Selecting;
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

const FSelectionStepResult& UBattleTargetingSession::GetCurrentStepResult() const
{
    return CurrentStepResult;
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
        if (ConfirmedSteps.IsEmpty() || !ConfirmedSteps.Last().HasSelectedCoord())
            return false;

        OutOriginCoord = ConfirmedSteps.Last().SelectedCoord;
        break;

    case ETargetingOriginSource::SpecificStep:
        if (!ConfirmedSteps.IsValidIndex(StepData->Origin.StepIndex) || !ConfirmedSteps[StepData->Origin.StepIndex].HasSelectedCoord())
            return false;

        OutOriginCoord = ConfirmedSteps[StepData->Origin.StepIndex].SelectedCoord;
        break;

    default:
        return false;
    }

    return IsValid(GridManager.Get()) && GridManager->IsValidCoord(OutOriginCoord);
}

bool UBattleTargetingSession::EvaluateCandidate(const FHexOffsetCoord& CandidateCoord, FSelectionStepResult& OutStepResult) const
{
    OutStepResult.Reset();

    const FTargetingStepCardData* StepData = GetCurrentStepData();

    if (!StepData || !StepData->Selection.RuleClass)
        return false;

    const UTargetSelection* Selection = StepData->Selection.RuleClass->GetDefaultObject<UTargetSelection>();

    if (!Selection)
        return false;

    FHexOffsetCoord OriginCoord;

    if (!GetCurrentOriginCoord(OriginCoord))
        return false;

    Selection->EvaluateCandidate(GridManager.Get(), OriginCoord, CandidateCoord, StepData->Selection.RuleData, OutStepResult);
    return OutStepResult.bValid;
}

bool UBattleTargetingSession::BuildCurrentStepIntent(FTargetingStepIntent& OutIntent) const
{
    OutIntent.Reset();

    if (!CurrentStepResult.bValid || !CurrentStepResult.HasOriginCoord())
        return false;

    OutIntent.SelectedCoord = CurrentStepResult.SelectedCoord;
    OutIntent.Direction = CurrentStepResult.SelectedDirection;

    if (CurrentStepResult.HasSelectedCoord())
    {
        OutIntent.RelativeOffset = FHexGridMath::OffsetToCube(CurrentStepResult.SelectedCoord) - FHexGridMath::OffsetToCube(CurrentStepResult.OriginCoord);

        if (const FBattleGridCell* Cell = GridManager->GetCellByCoord(GridWorldType, CurrentStepResult.SelectedCoord))
        {
            if (const ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(Cell->OccupyingActor.Get()))
                OutIntent.TargetCharacterKey = TargetCharacter->GetTargetingCharacterKey();
        }
    }

    const FTargetingStepCardData* StepData = GetCurrentStepData();

    if (StepData && StepData->Intent.Binding == ETargetingIntentBinding::TargetCharacter && !OutIntent.HasTargetCharacterKey())
        return false;

    return OutIntent.HasSelectedCoord() || OutIntent.Direction != INDEX_NONE || OutIntent.HasTargetCharacterKey();
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
