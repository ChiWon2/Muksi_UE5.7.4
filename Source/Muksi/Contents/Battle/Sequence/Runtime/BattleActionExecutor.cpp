#include "Muksi/Contents/Battle/Sequence/Runtime/BattleActionExecutor.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Execution/Core/BattleExecutionRunner.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Environment/BattleSequenceExecutionEnvironment.h"
#include "Muksi/Contents/Battle/StatusEffect/MuksiStatusEffectComponent.h"
#include "Muksi/Contents/Battle/Targeting/Resolver/BattleTargetResolver.h"

bool UBattleActionExecutor::Initialize(ABattleManager* InBattleManager, ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType)
{
	if (!IsValid(InGridManager)) return false;
	BattleManager = InBattleManager;
	GridManager = InGridManager;
	GridWorldType = InGridWorldType;
	return true;
}

bool UBattleActionExecutor::ExecuteAction(const FBattleSequenceRequest& Request)
{
	if (bRunning || !ValidateRequest(Request) || !IsValid(GridManager)) return false;

	FBattleAction SequenceAction = Request.Action;
	FResolvedTargeting ResolvedTargeting;
	if (!FBattleTargetResolver::ResolveAction(SequenceAction, GridManager, GridWorldType, ResolvedTargeting)) return false;

	CurrentAction = MoveTemp(SequenceAction);
	CurrentExecutionCard = Request.GetExecutionCard();
	CurrentResolvedTargeting = MoveTemp(ResolvedTargeting);
	ExecutionMode = Request.ExecutionMode;
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

bool UBattleActionExecutor::ValidateRequest(const FBattleSequenceRequest& Request) const
{
	UMuksiBattleCardDataAsset* ExecutionCard = Request.GetExecutionCard();
	return IsValid(Request.Action.Attacker.Get()) && IsValid(Request.Action.Card.Get()) && IsValid(ExecutionCard) && !ExecutionCard->MainExecutions.IsEmpty();
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
	Context.ResolvedTargeting = CurrentResolvedTargeting;
	Context.BattleGridManager = GridManager;
	Context.GridWorldType = GridWorldType;
	Context.NotifyKey = NotifyKey;
	return Context;
}

void UBattleActionExecutor::HandleExecutionEntryStarted(const FBattleExecutionEntry& Entry, int32 EntryIndex, FBattleExecutionContext& InOutExecutionContext)
{
	if (!bRunning) return;
	FResolvedTargeting RefreshedTargeting;
	if (FBattleTargetResolver::ResolveAction(CurrentAction, GridManager, GridWorldType, RefreshedTargeting)) CurrentResolvedTargeting = MoveTemp(RefreshedTargeting);
	else CurrentResolvedTargeting.Reset();
	InOutExecutionContext.ResolvedTargeting = CurrentResolvedTargeting;
	EntryStartedDelegate.Broadcast(CurrentAction, Entry, EntryIndex, CurrentResolvedTargeting);
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
	CurrentResolvedTargeting.Reset();
	ExecutionMode = EBattleExecutionMode::Sequence;
	AttackerAnimationComponent = nullptr;
	ActiveExecutionRunners.Reset();
	ExecutionEnvironment = nullptr;
}
