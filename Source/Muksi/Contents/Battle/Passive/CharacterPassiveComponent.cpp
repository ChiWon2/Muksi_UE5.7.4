// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"

#include "CharacterPassive.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"

// Sets default values for this component's properties
UCharacterPassiveComponent::UCharacterPassiveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}



void UCharacterPassiveComponent::InitializePassives(const TArray<TSubclassOf<UCharacterPassive>> PassiveClasses, ABattleManager* BattleManager)
{
	ActivePassives.Empty();

	OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());

	if (!IsValid(OwnerCharacter) ||
		PassiveClasses.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("PassiveClasses is num = 0 CharacterPassiveComponent.cpp"));
		return;
	}

	for (const TSubclassOf<UCharacterPassive>& PassiveClass
		: PassiveClasses)
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
		NewPassive->InitializePassive(OwnerCharacter);
		NewPassive->BindingEvent(BattleManager);
		ActivePassives.Add(NewPassive);
	}
}




void UCharacterPassiveComponent::CopyRuntimeStateFrom(const UCharacterPassiveComponent& SourceComponent)
{
	FinishRoundPhaseExecution();
	OwnerCharacter = Cast<ABattleCharacterBase>(GetOwner());
	ActivePassives.Reset();
	for (UCharacterPassive* SourcePassive : SourceComponent.ActivePassives)
	{
		if (!IsValid(SourcePassive)) continue;
		UCharacterPassive* NewPassive = NewObject<UCharacterPassive>(this, SourcePassive->GetClass());
		if (!IsValid(NewPassive)) continue;
		NewPassive->CopyRuntimeStateFrom(*SourcePassive, OwnerCharacter);
		ActivePassives.Add(NewPassive);
	}
}

void UCharacterPassiveComponent::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
	const TArray<TObjectPtr<UCharacterPassive>> PassivesSnapshot = ActivePassives;
	for (UCharacterPassive* Passive : PassivesSnapshot)
	{
		if (IsValid(Passive)) Passive->NotifyBattleActionStart(BattleAction);
	}
}

void UCharacterPassiveComponent::NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	const TArray<TObjectPtr<UCharacterPassive>> PassivesSnapshot = ActivePassives;
	for (UCharacterPassive* Passive : PassivesSnapshot)
	{
		if (IsValid(Passive)) Passive->NotifyBattlePhaseChanged(OldPhase, NewPhase);
	}
}

void UCharacterPassiveComponent::ExecuteRoundPhaseSequentially(
	EBattlePhase NewPhase,
	FSimpleDelegate CompletionDelegate)
{
	if (bExecutingRoundPhase)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterPassiveComponent] Round phase execution is already active."));
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	if (NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::RoundEnd)
	{
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	bExecutingRoundPhase = true;
	ExecutingRoundPhase = NewPhase;
	RoundPhaseExecutionQueue = ActivePassives;
	RoundPhaseExecutionIndex = 0;
	RoundPhaseCompletionDelegate = MoveTemp(CompletionDelegate);
	ExecuteNextRoundPhasePassive();
}

void UCharacterPassiveComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	FinishRoundPhaseExecution();
	Super::EndPlay(EndPlayReason);
}

void UCharacterPassiveComponent::ExecuteNextRoundPhasePassive()
{
	if (!bExecutingRoundPhase)
	{
		return;
	}

	while (RoundPhaseExecutionQueue.IsValidIndex(RoundPhaseExecutionIndex))
	{
		UCharacterPassive* Passive = RoundPhaseExecutionQueue[RoundPhaseExecutionIndex++];
		if (!IsValid(Passive))
		{
			continue;
		}

		Passive->ExecuteRoundPhase(
			ExecutingRoundPhase,
			FSimpleDelegate::CreateUObject(
				this,
				&UCharacterPassiveComponent::HandleRoundPhasePassiveFinished));
		return;
	}

	FinishRoundPhaseExecution();
}

void UCharacterPassiveComponent::HandleRoundPhasePassiveFinished()
{
	ExecuteNextRoundPhasePassive();
}

void UCharacterPassiveComponent::FinishRoundPhaseExecution()
{
	if (!bExecutingRoundPhase)
	{
		return;
	}

	bExecutingRoundPhase = false;
	ExecutingRoundPhase = EBattlePhase::None;
	RoundPhaseExecutionIndex = INDEX_NONE;
	RoundPhaseExecutionQueue.Reset();

	FSimpleDelegate CompletionDelegate = MoveTemp(RoundPhaseCompletionDelegate);
	RoundPhaseCompletionDelegate.Unbind();
	CompletionDelegate.ExecuteIfBound();
}
