// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"

void UCharacterPassive::BeginDestroy()
{
	NotifyRoundPhaseExecutionFinished();

	if (CachedBattleManager)
	{
		CachedBattleManager->ChangePhaseDelegate.RemoveDynamic(this, &UCharacterPassive::HandleChangePhaseDelegate);
		CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
		CachedBattleManager = nullptr;
	}

	Super::BeginDestroy();
}

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner)
{
	OwnerCharacter = InOwner;
}

void UCharacterPassive::CopyRuntimeStateFrom(const UCharacterPassive& SourcePassive, ABattleCharacterBase* InOwner)
{
	NotifyRoundPhaseExecutionFinished();
	if (CachedBattleManager) CachedBattleManager->ChangePhaseDelegate.RemoveDynamic(this, &UCharacterPassive::HandleChangePhaseDelegate);
	if (CachedBattleManager) CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
	CachedBattleManager = nullptr;
	OwnerCharacter = InOwner;
	Priority = SourcePassive.Priority;
	bEnabled = SourcePassive.bEnabled;
	bWaitForManualRoundPhaseCompletion = SourcePassive.bWaitForManualRoundPhaseCompletion;
}

void UCharacterPassive::BindingEvent(ABattleManager* BattleManager)
{
	if (CachedBattleManager)
	{
		CachedBattleManager->ChangePhaseDelegate.RemoveDynamic(this, &UCharacterPassive::HandleChangePhaseDelegate);
		CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
	}

	CachedBattleManager = BattleManager;
	if (CachedBattleManager)
	{
		CachedBattleManager->ChangePhaseDelegate.AddUniqueDynamic(this, &UCharacterPassive::HandleChangePhaseDelegate);
		CachedBattleManager->BattleActionStartDelegate.AddUObject(this, &UCharacterPassive::HandleBattleActionStart);
	}
}

void UCharacterPassive::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
	HandleBattleActionStart(BattleAction);
}

void UCharacterPassive::NotifyBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	if (!bEnabled || NewPhase == EBattlePhase::RoundStart || NewPhase == EBattlePhase::RoundEnd) return;
	HandleBattlePhaseChanged(OldPhase, NewPhase);
}

void UCharacterPassive::ExecuteRoundPhase(
	EBattlePhase NewPhase,
	FSimpleDelegate CompletionDelegate)
{
	if (bRoundPhaseExecutionActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterPassive] Round phase execution is already active."));
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	if (!bEnabled || (NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::RoundEnd))
	{
		CompletionDelegate.ExecuteIfBound();
		return;
	}

	bRoundPhaseExecutionActive = true;
	RoundPhaseCompletionDelegate = MoveTemp(CompletionDelegate);
	HandleBattlePhaseChanged(EBattlePhase::None, NewPhase);

	if (!bWaitForManualRoundPhaseCompletion)
	{
		NotifyRoundPhaseExecutionFinished();
	}
}

void UCharacterPassive::NotifyRoundPhaseExecutionFinished()
{
	if (!bRoundPhaseExecutionActive)
	{
		return;
	}

	bRoundPhaseExecutionActive = false;
	FSimpleDelegate CompletionDelegate = MoveTemp(RoundPhaseCompletionDelegate);
	RoundPhaseCompletionDelegate.Unbind();
	CompletionDelegate.ExecuteIfBound();
}

void UCharacterPassive::HandleChangePhaseDelegate(EBattlePhase OldPhase, EBattlePhase NewPhase)
{
	NotifyBattlePhaseChanged(OldPhase, NewPhase);
}

void UCharacterPassive::HandleBattlePhaseChanged(
	EBattlePhase OldPhase,
	EBattlePhase NewPhase)
{
	static_cast<void>(OldPhase);
	static_cast<void>(NewPhase);
}
