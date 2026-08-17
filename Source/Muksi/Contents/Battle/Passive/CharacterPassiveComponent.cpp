#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"
#include "CharacterPassive.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

UCharacterPassiveComponent::UCharacterPassiveComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}



void UCharacterPassiveComponent::InitializePassives(const TArray<TSubclassOf<UCharacterPassive>> PassiveClasses, ABattleManager* BattleManager)
{
	ActivePassives.Empty();

	OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());

	if (!IsValid(OwnerCharacter) || PassiveClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("PassiveClasses is num = 0 CharacterPassiveComponent.cpp"));
		return;
	}

	for (const TSubclassOf<UCharacterPassive>& PassiveClass : PassiveClasses)
	{
		if (!PassiveClass)
		{
			continue;
		}

		UCharacterPassive* NewPassive =
			NewObject<UCharacterPassive>(
				this,
				PassiveClass
			);

		if (!IsValid(NewPassive))
		{
			continue;
		}
		NewPassive->InitializePassive(OwnerCharacter, this);
		NewPassive->BindingEvent(BattleManager);
		ActivePassives.Add(NewPassive);
	}
}




void UCharacterPassiveComponent::CopyRuntimeStateFrom(const UCharacterPassiveComponent& SourceComponent)
{
	FinishExecution();
	OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());
	ActivePassives.Reset();
	for (UCharacterPassive* SourcePassive : SourceComponent.ActivePassives)
	{
		if (!IsValid(SourcePassive)) 
			continue;
		UCharacterPassive* NewPassive = NewObject<UCharacterPassive>(this, SourcePassive->GetClass());
		if (!IsValid(NewPassive)) 
			continue;
		
		NewPassive->CopyRuntimeStateFrom(*SourcePassive, OwnerCharacter, this);
		
		ActivePassives.Add(NewPassive);
	}
}

void UCharacterPassiveComponent::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
	const TArray<TObjectPtr<UCharacterPassive>> PassivesSnapshot = ActivePassives;
	for (UCharacterPassive* Passive : PassivesSnapshot)
	{
		if (IsValid(Passive)) 
			Passive->NotifyBattleActionStart(BattleAction);
	}
}

void UCharacterPassiveComponent::ExecuteSequentially(EBattlePhase OldPhase,EBattlePhase NewPhase, FSimpleDelegate CompletionDelegate, bool bInAllowDeferredCompletion)
{
	if (bExecuting)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterPassiveComponent] Execution is already active."));
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	bExecuting = true;
	ExecutingOldPhase = OldPhase;
	ExecutingNewPhase = NewPhase;
	ExecutionQueue = ActivePassives;
	ExecutionIndex = 0;
	bAllowDeferredCompletion = bInAllowDeferredCompletion;
	ExecutionCompletionDelegate = MoveTemp(CompletionDelegate);
	ExecuteNextPassive();
}

void UCharacterPassiveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FinishExecution();
	Super::EndPlay(EndPlayReason);
}

void UCharacterPassiveComponent::ExecuteNextPassive()
{
	if (!bExecuting)
	{
		return;
	}

	while (ExecutionQueue.IsValidIndex(ExecutionIndex))
	{
		UCharacterPassive* Passive = ExecutionQueue[ExecutionIndex++];
		if (!IsValid(Passive))
		{
			continue;
		}

		ExecutingPassive = Passive;
		Passive->Execute(ExecutingOldPhase, ExecutingNewPhase, bAllowDeferredCompletion);
		return;
	}

	FinishExecution();
}

void UCharacterPassiveComponent::NotifyPassiveExecutionFinished(UCharacterPassive* FinishedPassive)
{
	if (!bExecuting || FinishedPassive != ExecutingPassive)
	{
		return;
	}

	ExecutingPassive = nullptr;
	ExecuteNextPassive();
}

void UCharacterPassiveComponent::FinishExecution()
{
	if (!bExecuting)
	{
		return;
	}

	bExecuting = false;
	ExecutingOldPhase = EBattlePhase::None;
	ExecutingNewPhase = EBattlePhase::None;
	ExecutingPassive = nullptr;
	ExecutionIndex = INDEX_NONE;
	ExecutionQueue.Reset();
	bAllowDeferredCompletion = true;

	FSimpleDelegate CompletionDelegate = MoveTemp(ExecutionCompletionDelegate);
	ExecutionCompletionDelegate.Unbind();
	CompletionDelegate.ExecuteIfBound();
}
