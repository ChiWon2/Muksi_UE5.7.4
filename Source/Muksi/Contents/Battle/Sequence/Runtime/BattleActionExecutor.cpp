#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Environment/BattleSequenceExecutionEnvironment.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Targeting/CardData/TargetingCardData.h"
#include "Muksi/Contents/Battle/Targeting/Context/ResolvedStepResult.h"
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

bool UBattleActionExecutor::ExecuteAction(const FBattleAction& Action)
{
	if (bRunning || !ValidateAction(Action) || !IsValid(GridManager)) return false;

	FBattleAction SequenceAction = Action;
	FTargetingResult TargetingResult;
	if (!BuildActionTargetingResult(SequenceAction, TargetingResult)) return false;

	CurrentAction = MoveTemp(SequenceAction);
	CurrentExecutionCard = GetExecutionCard(CurrentAction);
	CurrentTargetingResult = MoveTemp(TargetingResult);
	ExecutionMode = BattleSimulationWorld::UsesSimulationRuntime(GridWorldType) ? EBattleExecutionMode::Simulation : EBattleExecutionMode::Sequence;
	bRunning = true;
	ActiveExecutionRunners.Reset();

	if (!InitializeExecutionEnvironment() || !BindAttackerNotify())
	{
		ResetRuntime();
		return false;
	}

	if (BattleManager && GridWorldType == EBattleSimulationWorldType::PlayerActualEnemyActual) BattleManager->NotifyBattleActionStart(CurrentAction);
	StartMainExecutionChain();
	return true;
}

void UBattleActionExecutor::Stop()
{
	if (!bRunning) return;
	ResetRuntime();
}

bool UBattleActionExecutor::BuildActionTargetingResult(const FBattleAction& Action, FTargetingResult& OutTargetingResult) const
{
	OutTargetingResult.Reset();
	if (!IsValid(Action.Attacker.Get()) || !IsValid(Action.Card.Get()) || !IsValid(GridManager)) return false;

	const FTargetingCardData& TargetingData = Action.Card->TargetingData;
	TArray<FResolvedStepResult> ResolvedSteps;
	if (!FBattleTargetResolver::ResolveIntent(Action.Attacker.Get(), GridManager, GridWorldType, TargetingData, Action.TargetingIntent, ResolvedSteps)) return false;

	OutTargetingResult.Steps.Reserve(ResolvedSteps.Num());
	for (int32 StepIndex = 0; StepIndex < ResolvedSteps.Num(); ++StepIndex)
	{
		const FTargetingStepCardData* StepData = TargetingData.GetStep(StepIndex);
		if (!StepData) return false;

		FTargetingStepResult StepResult;
		StepResult.ResolvedStep = ResolvedSteps[StepIndex];
		if (!StepResult.ResolvedStep.HasResolvedCoord()) return false;

		if (!StepData->Pattern.PatternClass)
		{
			StepResult.AffectedCoords.Add(StepResult.ResolvedStep.ResolvedCoord);
		}
		else
		{
			const UAreaPattern* Pattern = StepData->Pattern.PatternClass->GetDefaultObject<UAreaPattern>();
			if (!Pattern) return false;

			Pattern->ApplyPattern(
				GridManager,
				GridWorldType,
				StepData->Pattern.PatternData,
				StepResult.ResolvedStep.OriginCoord,
				StepResult.ResolvedStep.ResolvedCoord,
				StepResult.ResolvedStep.ResolvedDirection,
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
	UMuksiBattleCardDataAsset* ExecutionCard = GetExecutionCard(Action);
	return IsValid(Action.Attacker.Get()) && IsValid(Action.Card.Get()) && IsValid(ExecutionCard) && !ExecutionCard->MainExecutions.IsEmpty();
}

UMuksiBattleCardDataAsset* UBattleActionExecutor::GetExecutionCard(const FBattleAction& Action) const
{
	if (!IsValid(Action.Card.Get()) || BattleSimulationWorld::UsesActualCard(GridWorldType, Action.bPlayerAction)) return Action.Card.Get();
	UMuksiBattleCardDataAsset* DeceivedCard = Action.Card->GetDeceivedCard();
	return IsValid(DeceivedCard) ? DeceivedCard : Action.Card.Get();
}

bool UBattleActionExecutor::InitializeExecutionEnvironment()
{
	ExecutionEnvironment = NewObject<UBattleSequenceExecutionEnvironment>(this);
	if (!ExecutionEnvironment) return false;
	ExecutionEnvironment->InitializeSequence(CurrentAction.Attacker, CurrentExecutionCard, GridManager);
	return ExecutionEnvironment->IsValidEnvironment();
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

void UBattleActionExecutor::StartMainExecutionChain()
{
	if (!CurrentExecutionCard || CurrentExecutionCard->MainExecutions.IsEmpty())
	{
		Finish();
		return;
	}

	TArray<FBattleExecutionEntry> MainExecutions;
	if (ExecutionMode == EBattleExecutionMode::Sequence && CurrentAction.Attacker)
	{
		if (UMuksiStatusEffectComponent* StatusEffectComponent = CurrentAction.Attacker->GetStatusEffectComponent()) StatusEffectComponent->AppendBattleActionStartExecutions(CurrentAction, MainExecutions);
	}
	MainExecutions.Append(CurrentExecutionCard->MainExecutions);
	StartExecutionRunner(MainExecutions, MakeExecutionContext(NAME_None));
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
		if (NotifyChain.IsValid() && NotifyChain.NotifyKey == NotifyKey) StartExecutionRunner(NotifyChain.Executions, MakeExecutionContext(NotifyKey));
	}
}

void UBattleActionExecutor::StartExecutionRunner(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	if (!bRunning || ExecutionEntries.IsEmpty()) return;
	UBattleExecutionRunner* Runner = NewObject<UBattleExecutionRunner>(this);
	if (!Runner) return;
	ActiveExecutionRunners.Add(Runner);
	FBattleExecutionEntryStarted OnEntryStarted;
	OnEntryStarted.BindUObject(this, &UBattleActionExecutor::HandleExecutionEntryStarted);
	FBattleExecutionRunnerFinished OnFinished;
	OnFinished.BindUObject(this, &UBattleActionExecutor::HandleExecutionRunnerFinished);
	Runner->Run(ExecutionEntries, Context, OnEntryStarted, FBattleExecutionEntryFinished(), OnFinished);
}

FBattleExecutionContext UBattleActionExecutor::MakeExecutionContext(FName NotifyKey) const
{
	FBattleExecutionContext Context;
	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentExecutionCard;
	Context.ExecutionMode = ExecutionMode;
	Context.Environment = ExecutionEnvironment;
	Context.TargetingResult = CurrentTargetingResult;
	Context.BattleGridManager = GridManager;
	Context.GridWorldType = GridWorldType;
	Context.NotifyKey = NotifyKey;
	return Context;
}

void UBattleActionExecutor::HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext)
{
	if (!bRunning) return;
	FTargetingResult RefreshedTargeting;
	if (BuildActionTargetingResult(CurrentAction, RefreshedTargeting)) CurrentTargetingResult = MoveTemp(RefreshedTargeting);
	else CurrentTargetingResult.Reset();
	InOutExecutionContext.TargetingResult = CurrentTargetingResult;
	EntryStartedDelegate.Broadcast(CurrentAction, Entry, EntryIndex, CurrentTargetingResult);
}

void UBattleActionExecutor::HandleExecutionRunnerFinished(UBattleExecutionRunner* FinishedRunner)
{
	if (!bRunning || !FinishedRunner || ActiveExecutionRunners.RemoveSingle(FinishedRunner) == 0) return;
	TryFinish();
}

void UBattleActionExecutor::TryFinish()
{
	if (bRunning && ActiveExecutionRunners.IsEmpty()) Finish();
}

void UBattleActionExecutor::Finish()
{
	if (!bRunning) return;
	ResetRuntime();
	FinishedDelegate.Broadcast();
}

void UBattleActionExecutor::ResetRuntime()
{
	UnbindAttackerNotify();
	bRunning = false;
	CurrentAction = FBattleAction();
	CurrentExecutionCard = nullptr;
	CurrentTargetingResult.Reset();
	AttackerAnimationComponent = nullptr;
	ActiveExecutionRunners.Reset();
	ExecutionEnvironment = nullptr;
}
