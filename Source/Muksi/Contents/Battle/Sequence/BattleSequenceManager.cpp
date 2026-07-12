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
	PendingExecutionCount = 0;
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
	if (!CurrentAction.Attacker)
	{
		return false;
	}

	AttackerAnimationComponent = CurrentAction.Attacker->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!AttackerAnimationComponent)
	{
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

	ExecuteNotifyExecutionBinding(NotifyKey);
}

void ABattleSequenceManager::ExecuteNotifyExecutionBinding(FName NotifyKey)
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
			return;
		}

		ExecutionChain->InitializeChain(Binding.ExecutionChain);
		ExecuteExecutionInstanceWithContext(ExecutionChain, MakeExecutionContext(NotifyKey));
		return;
	}
}

void ABattleSequenceManager::ExecuteExecutionClass(TSubclassOf<UMuksiBattleExecution> ExecutionClass, FName NotifyKey)
{
	ExecuteExecutionClassWithContext(ExecutionClass, MakeExecutionContext(NotifyKey));
}

void ABattleSequenceManager::ExecuteExecutionClassWithContext(TSubclassOf<UMuksiBattleExecution> ExecutionClass, const FMuksiBattleExecutionContext& Context)
{
	if (!bSequenceRunning || !ExecutionClass)
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
	if (!bSequenceRunning || !Execution)
	{
		return;
	}

	RunningExecutions.Add(Execution);
	++PendingExecutionCount;

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Execution Started. Execution=%s Pending=%d"), *GetNameSafe(Execution), PendingExecutionCount);

	FMuksiBattleExecutionFinished OnFinished;
	OnFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionFinished);

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

void ABattleSequenceManager::HandleExecutionFinished()
{
	PendingExecutionCount = FMath::Max(0, PendingExecutionCount - 1);

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Execution Finished. Pending=%d"), PendingExecutionCount);

	TryFinishSequence();
}

void ABattleSequenceManager::TryFinishSequence()
{
	if (!bSequenceRunning || PendingExecutionCount > 0)
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
	PendingExecutionCount = 0;
	CurrentAction = FBattleAction();
	AttackerAnimationComponent = nullptr;
	RunningExecutions.Empty();

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Sequence Finished."));

	OnSequenceFinished.Broadcast();
}