#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

bool UBattleActionExecutor::Initialize(ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType)
{
	if (!IsValid(InGridManager))
		return false;
	GridManager = InGridManager;
	GridWorldType = InGridWorldType;
	return true;
}

bool UBattleActionExecutor::ExecuteBattleAction(const FBattleAction& Action)
{
	if (bRunning || !ValidateAction(Action) || !IsValid(GridManager))
		return false;

	UMuksiBattleCardDataAsset* ExecutionCard = ResolveExecutionCard(Action);
	if (!IsValid(ExecutionCard) || Action.ExecutionEntries.IsEmpty())
		return false;

	FTargetingResult TargetingResult;
	if (!ResolveActionTargetingResult(Action, TargetingResult))
		return false;

	CurrentAction = Action;
	CurrentExecutionCard = ExecutionCard;
	ActionTargetingResult = MoveTemp(TargetingResult);
	bRunning = true;
	bStopAfterCurrentExecution = false;
	ActiveExecutionRunners.Reset();

	if (!BindExecutionNotifySources())
	{
		ResetRuntime();
		return false;
	}

	if (!RunMainExecutionEntries())
	{
		ResetRuntime();
		return false;
	}
	return true;
}

void UBattleActionExecutor::StopAfterCurrentExecution()
{
	if (!bRunning || bStopAfterCurrentExecution)
		return;

	bStopAfterCurrentExecution = true;
	UnbindExecutionNotifySources();

	for (UBattleExecutionRunner* Runner : ActiveExecutionRunners)
	{
		if (Runner)
			Runner->StopAfterCurrentExecution();
	}
}

void UBattleActionExecutor::Stop()
{
	if (!bRunning)
		return;
	ResetRuntime();
}

bool UBattleActionExecutor::ResolveActionTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const
{
	OutTargetingResult.Reset();
	const FTargetingCardData& TargetingData = Action.Card->TargetingData;
	TArray<FTargetingStep> ResolvedSteps;
	if (!FBattleTargetResolver::ResolveIntent(Action.Attacker.Get(), GridManager, GridWorldType, TargetingData, Action.TargetingIntent, ResolvedSteps))
		return false;

	OutTargetingResult.Steps.Reserve(ResolvedSteps.Num());
	for (int32 StepIndex = 0; StepIndex < ResolvedSteps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData* StepData = TargetingData.GetStep(StepIndex);
		if (!StepData)
			return false;

		FTargetingStepResult StepResult;
		StepResult.Step = ResolvedSteps[StepIndex];
		if (!StepResult.Step.HasTargetCoord())
			return false;

		if (!StepData->Pattern.PatternClass)
		{
			StepResult.AffectedCoords.Add(StepResult.Step.TargetCoord);
		}
		else
		{
			const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
			if (!Pattern)
				return false;

			Pattern->ApplyPattern(
				GridManager,
				GridWorldType,
				StepData->Pattern.PatternData,
				StepResult.Step.OriginCoord,
				StepResult.Step.TargetCoord,
				StepResult.Step.Direction,
				StepResult.AffectedCoords,
				StepResult.PathCoords);
		}

		GridManager->GetCharactersAtCoords(GridWorldType, StepResult.AffectedCoords, StepResult.Targets);
		OutTargetingResult.Steps.Add(MoveTemp(StepResult));
	}
	return true;
}

bool UBattleActionExecutor::ValidateAction(const FBattleAction& Action) const
{
	return IsValid(Action.Attacker.Get()) && IsValid(Action.Card.Get());
}

UMuksiBattleCardDataAsset* UBattleActionExecutor::ResolveExecutionCard(const FBattleAction& Action) const
{
	if (!BattleSimulationWorld::UsesActualCard(GridWorldType, Action.bPlayerAction))
		return Action.Card.Get();

	UMuksiBattleCardDataAsset* ActualCard = Action.Card->GetActualCard();
	return IsValid(ActualCard) ? ActualCard : Action.Card.Get();
}

bool UBattleActionExecutor::BindExecutionNotifySources()
{
	NotifyAnimationComponents.Reset();

	for (const FBattleExecutionNotify& ExecutionNotify : CurrentAction.ExecutionNotifies)
	{
		if (!ExecutionNotify.IsValid())
			continue;

		ABattleCharacterBase* NotifySource = ResolveNotifySource(ExecutionNotify);
		if (!IsValid(NotifySource))
			return false;

		UMuksiBattleAnimationComponent* AnimationComponent = NotifySource->FindComponentByClass<UMuksiBattleAnimationComponent>();
		if (!IsValid(AnimationComponent))
			return false;

		if (NotifyAnimationComponents.Contains(AnimationComponent))
			continue;

		AnimationComponent->OnBattleExecutionNotifyWithSource.AddUniqueDynamic(this, &UBattleActionExecutor::HandleBattleExecutionNotify);
		NotifyAnimationComponents.Add(AnimationComponent);
	}

	return true;
}

void UBattleActionExecutor::UnbindExecutionNotifySources()
{
	for (UMuksiBattleAnimationComponent* AnimationComponent : NotifyAnimationComponents)
	{
		if (IsValid(AnimationComponent))
			AnimationComponent->OnBattleExecutionNotifyWithSource.RemoveDynamic(this, &UBattleActionExecutor::HandleBattleExecutionNotify);
	}

	NotifyAnimationComponents.Reset();
}

ABattleCharacterBase* UBattleActionExecutor::ResolveNotifySource(const FBattleExecutionNotify& ExecutionNotify) const
{
	return ExecutionNotify.NotifySourceOverride
		? ExecutionNotify.NotifySourceOverride.Get()
		: CurrentAction.Attacker.Get();
}

bool UBattleActionExecutor::RunMainExecutionEntries()
{
	const TArray<FBattleExecutionEntry> MainExecutionEntries = CurrentAction.ExecutionEntries;
	return RunExecutionEntries(MainExecutionEntries);
}

void UBattleActionExecutor::HandleBattleExecutionNotify(ABattleCharacterBase* NotifySource, FName NotifyKey)
{
	if (bRunning && !bStopAfterCurrentExecution && IsValid(NotifySource) && !NotifyKey.IsNone())
		RunExecutionEntriesForNotify(NotifySource, NotifyKey);
}

void UBattleActionExecutor::RunExecutionEntriesForNotify(ABattleCharacterBase* NotifySource, FName NotifyKey)
{
	for (const FBattleExecutionNotify& ExecutionNotify : CurrentAction.ExecutionNotifies)
	{
		if (!ExecutionNotify.IsValid() || ExecutionNotify.NotifyKey != NotifyKey)
			continue;

		if (ResolveNotifySource(ExecutionNotify) != NotifySource)
			continue;

		RunExecutionEntries(ExecutionNotify.ExecutionEntries, NotifySource);
	}
}

bool UBattleActionExecutor::RunExecutionEntries(const TArray<FBattleExecutionEntry>& ExecutionEntries, ABattleCharacterBase* ExecutionSource)
{
	if (!bRunning || bStopAfterCurrentExecution || ExecutionEntries.IsEmpty())
		return false;

	UBattleExecutionRunner* Runner = NewObject<UBattleExecutionRunner>(this);
	if (!Runner)
		return false;

	FBattleExecutionContext Context;
	Context.Attacker = IsValid(ExecutionSource) ? ExecutionSource : CurrentAction.Attacker.Get();
	Context.Card = CurrentExecutionCard;
	Context.ExecutionMode = BattleSimulationWorld::UsesSimulationRuntime(GridWorldType)? EBattleExecutionMode::Simulation : EBattleExecutionMode::ActualBattle;
	Context.TargetingResult = ActionTargetingResult;
	Context.BattleGridManager = GridManager;
	Context.GridWorldType = GridWorldType;

	ActiveExecutionRunners.Add(Runner);

	FBattleExecutionEntryStarted OnEntryStarted;
	OnEntryStarted.BindUObject(this, &UBattleActionExecutor::HandleExecutionEntryStarted);

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UBattleActionExecutor::HandleExecutionRunnerFinished);

	Runner->RunExecutionEntries(ExecutionEntries, Context, OnEntryStarted, FBattleExecutionEntryFinished(), OnFinished);
	return true;
}

void UBattleActionExecutor::HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext)
{
	if (!bRunning)
		return;

	// Action 시작 시 Resolve한 Targeting 스냅샷을 모든 Entry에 유지한다.
	InOutExecutionContext.TargetingResult = ActionTargetingResult;
	OnExecutionEntryStarted.ExecuteIfBound(CurrentAction, Entry, EntryIndex, ActionTargetingResult);
}

void UBattleActionExecutor::HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bRunning || !FinishedRunner || ActiveExecutionRunners.RemoveSingle(FinishedRunner) == 0)
		return;
	TryCompleteAction();
}

void UBattleActionExecutor::TryCompleteAction()
{
	if (bRunning && ActiveExecutionRunners.IsEmpty())
		CompleteAction();
}

void UBattleActionExecutor::CompleteAction()
{
	if (!bRunning)
		return;
	ResetRuntime();
	OnBattleActionCompleted.ExecuteIfBound();
}

void UBattleActionExecutor::ResetRuntime()
{
	UnbindExecutionNotifySources();
	bRunning = false;
	bStopAfterCurrentExecution = false;
	CurrentAction = FBattleAction();
	CurrentExecutionCard = nullptr;
	ActionTargetingResult.Reset();
	NotifyAnimationComponents.Reset();
	ActiveExecutionRunners.Reset();
}
