#include "Muksi/Contents/Battle/Sequence/BattleSequenceManager.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/BattleExecutionChain.h"

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
	ActiveExecutionChains.Empty();

	if (!BindAttackerNotify())
	{
		FinishSequence();
		return false;
	}

	StartMainExecutionChain();
	return true;
}

bool ABattleSequenceManager::ValidateAction(const FBattleAction& InAction) const
{
	return IsValid(InAction.Attacker) && IsValid(InAction.Card) && !InAction.Card->MainExecutions.IsEmpty();
}

bool ABattleSequenceManager::BindAttackerNotify()
{
	if (!CurrentAction.Card || CurrentAction.Card->NotifyExecutionChains.IsEmpty())
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

void ABattleSequenceManager::StartMainExecutionChain()
{
	if (!CurrentAction.Card || CurrentAction.Card->MainExecutions.IsEmpty())
	{
		FinishSequence();
		return;
	}

	StartExecutionChain(CurrentAction.Card->MainExecutions, MakeExecutionContext(NAME_None));
}

void ABattleSequenceManager::HandleBattleExecutionNotify(FName NotifyKey)
{
	if (!bSequenceRunning || NotifyKey.IsNone())
	{
		return;
	}

	StartNotifyExecutionChains(NotifyKey);
}

void ABattleSequenceManager::StartNotifyExecutionChains(FName NotifyKey)
{
	if (!CurrentAction.Card)
	{
		return;
	}

	for (const FBattleNotifyExecutionChain& NotifyChain : CurrentAction.Card->NotifyExecutionChains)
	{
		if (!NotifyChain.IsValid() || NotifyChain.NotifyKey != NotifyKey)
		{
			continue;
		}

		StartExecutionChain(NotifyChain.Executions, MakeExecutionContext(NotifyKey));
	}
}

void ABattleSequenceManager::StartExecutionChain(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	if (!bSequenceRunning || ExecutionEntries.IsEmpty())
	{
		return;
	}

	UBattleExecutionChain* ExecutionChain = NewObject<UBattleExecutionChain>(this);

	if (!ExecutionChain)
	{
		return;
	}

	ExecutionChain->InitializeChain(ExecutionEntries);
	ActiveExecutionChains.Add(ExecutionChain);

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionChain Started. Chain=%s ActiveChains=%d"), *GetNameSafe(ExecutionChain), ActiveExecutionChains.Num());

	FBattleExecutionFinished OnFinished;
	OnFinished.BindUObject(this, &ABattleSequenceManager::HandleExecutionChainFinished, ExecutionChain);

	ExecutionChain->Execute(Context, OnFinished);
}

void ABattleSequenceManager::HandleRuntimeExecutionChainRequested(const TArray<FBattleExecutionEntry>& ExecutionEntries, const FBattleExecutionContext& Context)
{
	StartExecutionChain(ExecutionEntries, Context);
}

FBattleExecutionContext ABattleSequenceManager::MakeExecutionContext(FName NotifyKey)
{
	FBattleExecutionContext Context;

	Context.Attacker = CurrentAction.Attacker;
	Context.Card = CurrentAction.Card;
	Context.BattleGridManager = BattleGridManager;
	Context.TargetPoints = CurrentAction.TargetingResult.AffectedCoords;
	Context.NotifyKey = NotifyKey;
	Context.RequestRuntimeExecutionChain.BindUObject(this, &ABattleSequenceManager::HandleRuntimeExecutionChainRequested);

	if (Context.TargetPoints.IsEmpty() && CurrentAction.TargetingResult.HasSelectedCoord())
	{
		Context.TargetPoints.Add(CurrentAction.TargetingResult.GetSelectedCoord());
	}

	return Context;
}

void ABattleSequenceManager::HandleExecutionChainFinished(UBattleExecutionChain* FinishedChain)
{
	if (!bSequenceRunning || !FinishedChain)
	{
		return;
	}

	const int32 RemovedCount = ActiveExecutionChains.RemoveSingle(FinishedChain);

	if (RemovedCount == 0)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] ExecutionChain Finished. Chain=%s ActiveChains=%d"), *GetNameSafe(FinishedChain), ActiveExecutionChains.Num());

	TryFinishSequence();
}

void ABattleSequenceManager::TryFinishSequence()
{
	if (!bSequenceRunning || !ActiveExecutionChains.IsEmpty())
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
	ActiveExecutionChains.Empty();

	UE_LOG(LogTemp, Log, TEXT("[BattleSequenceManager] Sequence Finished."));

	OnSequenceFinished.Broadcast();
}