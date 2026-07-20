#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/MuksiBattleExecutionChain.h"

ABattleSequenceManager::ABattleSequenceManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

bool ABattleSequenceManager::StartSequence(const FBattleAction& InAction)
{
	if (bSequenceRunning || !ValidateAction(InAction))
	{
		return false;
	}

	CurrentAction = InAction;
	bSequenceRunning = true;
	RunningExecutions.Empty();

	if (!BindAttackerNotify())
	{
		FinishSequence();
		return false;
	}

	StartInitialExecutionChain();
	return true;
}

bool ABattleSequenceManager::ValidateAction(const FBattleAction& InAction) const
{
	return IsValid(InAction.Attacker) && IsValid(InAction.Card) && !InAction.Card->ExecutionChain.IsEmpty();
}

bool ABattleSequenceManager::BindAttackerNotify()
{
	if (!CurrentAction.Card || CurrentAction.Card->NotifyExecutionBindings.IsEmpty())
	{
		return true;
	}

	if (!CurrentAction.Attacker)
	{
		return false;
	}

	AttackerAnimationComponent = CurrentAction.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AttackerAnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleSequenceManager] AnimationComponent not found. Attacker=%s"), *GetNameSafe(CurrentAction.Attacker.Get()));
		return false;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.AddUniqueDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
	return true;
}

void ABattleSequenceManager::UnbindAttackerNotify()
{
	if (!AttackerAnimationComponent)
	{
		return;
	}

	AttackerAnimationComponent->OnBattleExecutionNotify.RemoveDynamic(this, &ABattleSequenceManager::HandleBattleExecutionNotify);
}

void ABattleSequenceManager::StartInitialExecutionChain()
{
	if (!CurrentAction.Card || CurrentAction.Card->ExecutionChain.IsEmpty())
	{
		FinishSequence();
		return;
	}

	UMuksiBattleExecutionChain* ExecutionChain = NewObject<UMuksiBattleExecutionChain>(this);

	if (!ExecutionChain)
	{
		FinishSequence();
		return;
	}

	ExecutionChain->InitializeChain(CurrentAction.Card->ExecutionChain);
	ExecuteExecutionInstanceWithContext(ExecutionChain, MakeExecutionContext(NAME_None));
}

void ABattleSequenceManager::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (!bSequenceRunning || NotifyKey.IsNone())
	{
		return;
	}

	ExecuteNotifyExecutionBindings(NotifyKey);
}

void ABattleSequenceManager::ExecuteNotifyExecutionBindings(FName NotifyKey)
{
	if (!CurrentAction.Card)
	{
		return;
	}

	for (const FMuksiBattleNotifyExecutionBinding& Binding : CurrentAction.Card->NotifyExecutionBindings)
	{
		if (!Binding.IsValid() || Binding.NotifyKey != NotifyKey)
		{
			continue;
		}

		UMuksiBattleExecutionChain* ExecutionChain = NewObject<UMuksiBattleExecutionChain>(this);

		if (!ExecutionChain)
		{
			continue;
		}

		ExecutionChain->InitializeChain(Binding.ExecutionChain);
		ExecuteExecutionInstanceWithContext(ExecutionChain, MakeExecutionContext(NotifyKey));
	}
}

void ABattleSequenceManager::ExecuteExecutionClass(TSubclassOf<UMuksiBattleExecution> ExecutionClass, FName NotifyKey)
{
	ExecuteExecutionClassWithContext(ExecutionClass, MakeExecutionContext(NotifyKey));
}

void ABattleSequenceManager::ExecuteExecutionClassWithContext(TSubclassOf<UMuksiBattleExecution> ExecutionClass, const FMuksiBattleExecutionContext& Context)
{
	if (!bSequenceRunning || !ExecutionClass || ExecutionClass->HasAnyClassFlags(CLASS_Abstract))
	{
		return;
	}

	UMuksiBattleExecution* Execution = NewObject<UMuksiBattleExecution>(this, ExecutionClass);

	if (!Execution)
	{
		return;
	}

	ExecuteExecutionInstanceWithContext(Execution, Context);
}

void ABattleSequenceManager::ExecuteExecutionInstanceWithContext(UMuksiBattleExecution* Execution, const FMuksiBattleExecutionContext& Context)
{
	if (!bSequenceRunning || !Execution || RunningExecutions.Contains(Execution))
	{
		return;
	}

	RunningExecutions.Add(Execution);

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Execution Started. Execution=%s Running=%d"), *GetNameSafe(Execution), RunningExecutions.Num());

	FMuksiBattleExecutionFinished OnFinished;
	OnFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionFinished, Execution);

	Execution->Execute(Context, OnFinished);
}

void ABattleSequenceManager::HandleSystemExecutionRequested(TSubclassOf<UMuksiBattleExecution> ExecutionClass, const FMuksiBattleExecutionContext& Context)
{
	ExecuteExecutionClassWithContext(ExecutionClass, Context);
}

FMuksiBattleExecutionContext ABattleSequenceManager::MakeExecutionContext(FName NotifyKey)
{
	FMuksiBattleExecutionContext Context;

	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentAction.Card;
	Context.BattleGridManager = BattleGridManager;
	Context.TargetPoints = CurrentAction.TargetPoints;
	Context.NotifyKey = NotifyKey;
	Context.RequestSystemExecution.BindUObject(this, &ABattleSequenceManager::HandleSystemExecutionRequested);

	return Context;
}

void ABattleSequenceManager::HandleExecutionFinished(UMuksiBattleExecution* FinishedExecution)
{
	if (!bSequenceRunning || !FinishedExecution)
	{
		return;
	}

	const int32 RemovedCount = RunningExecutions.RemoveSingle(FinishedExecution);

	if (RemovedCount == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Execution Finished. Execution=%s Running=%d"), *GetNameSafe(FinishedExecution), RunningExecutions.Num());

	TryFinishSequence();
}

void ABattleSequenceManager::TryFinishSequence()
{
	if (!bSequenceRunning || !RunningExecutions.IsEmpty())
	{
		return;
	}

	FinishSequence();
}

void ABattleSequenceManager::FinishSequence()
{
	if (!bSequenceRunning)
	{
		return;
	}

	UnbindAttackerNotify();

	bSequenceRunning = false;
	CurrentAction = FBattleAction();
	AttackerAnimationComponent = nullptr;
	RunningExecutions.Empty();

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Sequence Finished."));

	OnSequenceFinished.Broadcast();
}