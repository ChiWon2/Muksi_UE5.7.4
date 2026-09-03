#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStep.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/AreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

bool UBattleActionExecutor::Initialize(ABattleManager* InBattleManager, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType)
{
	if (!IsValid(InGridManager)) return false;
	BattleManager = InBattleManager;
	GridManager = InGridManager;
	GridWorldType = InGridWorldType;
	return true;
}

bool UBattleActionExecutor::ExecuteBattleAction(const FBattleAction& Action)
{
	if (bRunning || !ValidateAction(Action) || !IsValid(GridManager)) return false;

	UMuksiBattleCardDataAsset* ExecutionCard = ResolveExecutionCard(Action);
	if (!IsValid(ExecutionCard) || ExecutionCard->MainExecutions.IsEmpty()) return false;

	FTargetingResult TargetingResult;
	if (!ResolveActionTargetingResult(Action, TargetingResult)) return false;

	CurrentAction = Action;
	CurrentExecutionCard = ExecutionCard;
	ActionTargetingResult = MoveTemp(TargetingResult);
	bRunning = true;
	ActiveExecutionRunners.Reset();

	if (!BindAttackerNotify())
	{
		ResetRuntime();
		return false;
	}

	if (BattleManager && GridWorldType == EBattleSimulationWorldType::PlayerActualEnemyActual) BattleManager->NotifyBattleActionStart(CurrentAction);
	if (!StartMainExecutions())
	{
		ResetRuntime();
		return false;
	}
	return true;
}

void UBattleActionExecutor::Stop()
{
	if (!bRunning) return;
	ResetRuntime();
}

bool UBattleActionExecutor::ResolveActionTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const
{
	OutTargetingResult.Reset();
	const FTargetingCardData& TargetingData = Action.Card->TargetingData;
	TArray<FTargetingStep> ResolvedSteps;
	if (!FBattleTargetResolver::ResolveIntent(Action.Attacker.Get(), GridManager, GridWorldType, TargetingData, Action.TargetingIntent, ResolvedSteps)) return false;

	OutTargetingResult.Steps.Reserve(ResolvedSteps.Num());
	for (int32 StepIndex = 0; StepIndex < ResolvedSteps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData* StepData = TargetingData.GetStep(StepIndex);
		if (!StepData) return false;

		FTargetingStepResult StepResult;
		StepResult.Step = ResolvedSteps[StepIndex];
		if (!StepResult.Step.HasTargetCoord()) return false;

		if (!StepData->Pattern.PatternClass)
		{
			StepResult.AffectedCoords.Add(StepResult.Step.TargetCoord);
		}
		else
		{
			const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
			if (!Pattern) return false;

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
	if (BattleSimulationWorld::UsesActualCard(GridWorldType, Action.bPlayerAction)) return Action.Card.Get();
	UMuksiBattleCardDataAsset* DeceivedCard = Action.Card->GetDeceivedCard();
	return IsValid(DeceivedCard) ? DeceivedCard : Action.Card.Get();
}

bool UBattleActionExecutor::BindAttackerNotify()
{
	if (!CurrentExecutionCard || CurrentExecutionCard->NotifyExecutionChains.IsEmpty()) return true;
	if (!CurrentAction.Attacker) return false;
	AttackerAnimationComponent = CurrentAction.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();
	if (!AttackerAnimationComponent) return false;
	AttackerAnimationComponent->OnBattleExecutionNotify.AddUniqueDynamic(this, &UBattleActionExecutor::HandleBattleExecutionNotify);
	return true;
}

void UBattleActionExecutor::UnbindAttackerNotify()
{
	if (AttackerAnimationComponent) AttackerAnimationComponent->OnBattleExecutionNotify.RemoveDynamic(this, &UBattleActionExecutor::HandleBattleExecutionNotify);
}

bool UBattleActionExecutor::StartMainExecutions()
{
	TArray<FBattleExecutionEntry> MainExecutions;
	if (!BattleSimulationWorld::UsesSimulationRuntime(GridWorldType) && CurrentAction.Attacker)
	{
		if (UMuksiStatusEffectComponent* StatusEffectComponent = CurrentAction.Attacker->GetStatusEffectComponent())
		{
			StatusEffectComponent->AppendBattleActionStartExecutions(CurrentAction, MainExecutions);
		}
	}

	MainExecutions.Append(CurrentExecutionCard->MainExecutions);
	return RunExecutionSequence(MainExecutions);
}

void UBattleActionExecutor::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (bRunning && !NotifyKey.IsNone()) StartNotifyExecutionChains(NotifyKey);
}

void UBattleActionExecutor::StartNotifyExecutionChains(FName NotifyKey)
{
	if (!CurrentExecutionCard) return;
	for (const FBattleNotifyExecutionChain& NotifyChain : CurrentExecutionCard->NotifyExecutionChains)
	{
		if (NotifyChain.IsValid() && NotifyChain.NotifyKey == NotifyKey) RunExecutionSequence(NotifyChain.Executions);
	}
}

bool UBattleActionExecutor::RunExecutionSequence(const TArray<FBattleExecutionEntry>& ExecutionEntries)
{
	if (!bRunning || ExecutionEntries.IsEmpty()) return false;

	UBattleExecutionRunner* Runner = NewObject<UBattleExecutionRunner>(this);
	if (!Runner) return false;

	FBattleExecutionContext Context;
	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentExecutionCard;
	Context.ExecutionMode = BattleSimulationWorld::UsesSimulationRuntime(GridWorldType)
		? EBattleExecutionMode::Simulation
		: EBattleExecutionMode::Sequence;
	Context.TargetingResult = ActionTargetingResult;
	Context.BattleGridManager = GridManager;
	Context.GridWorldType = GridWorldType;

	ActiveExecutionRunners.Add(Runner);

	FBattleExecutionEntryStarted OnEntryStarted;
	OnEntryStarted.BindUObject(this, &UBattleActionExecutor::HandleExecutionEntryStarted);

	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UBattleActionExecutor::HandleExecutionSequenceFinished);

	Runner->RunExecutions(ExecutionEntries, Context, OnEntryStarted, FBattleExecutionEntryFinished(), OnFinished);
	return true;
}

void UBattleActionExecutor::HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext)
{
	if (!bRunning) return;

	// Action 시작 시 Resolve한 Targeting 스냅샷을 모든 Entry에 유지한다.
	InOutExecutionContext.TargetingResult = ActionTargetingResult;
	OnBattleExecutionStarted.ExecuteIfBound(CurrentAction, Entry, EntryIndex, ActionTargetingResult);
}

void UBattleActionExecutor::HandleExecutionSequenceFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bRunning || !FinishedRunner || ActiveExecutionRunners.RemoveSingle(FinishedRunner) == 0) return;
	TryCompleteAction();
}

void UBattleActionExecutor::TryCompleteAction()
{
	if (bRunning && ActiveExecutionRunners.IsEmpty()) CompleteAction();
}

void UBattleActionExecutor::CompleteAction()
{
	if (!bRunning) return;
	ResetRuntime();
	OnBattleActionCompleted.ExecuteIfBound();
}

void UBattleActionExecutor::ResetRuntime()
{
	UnbindAttackerNotify();
	bRunning = false;
	CurrentAction = FBattleAction();
	CurrentExecutionCard = nullptr;
	ActionTargetingResult.Reset();
	AttackerAnimationComponent = nullptr;
	ActiveExecutionRunners.Reset();
}
