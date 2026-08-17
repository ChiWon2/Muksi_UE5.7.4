// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Data/BattleAction.h"
#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"

void UCharacterPassive::BeginDestroy()
{
	CompleteExecution();

	if (CachedBattleManager)
	{
		CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
		CachedBattleManager = nullptr;
	}

	Super::BeginDestroy();
}

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner, UCharacterPassiveComponent* InOwnerComponent)
{
	OwnerCharacter = InOwner;
	OwnerComponent = InOwnerComponent;
}

void UCharacterPassive::CopyRuntimeStateFrom(const UCharacterPassive& SourcePassive, ABattleCharacterBase* InOwner, UCharacterPassiveComponent* InOwnerComponent)
{
	CompleteExecution();
	if (CachedBattleManager) CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
	CachedBattleManager = nullptr;
	OwnerCharacter = InOwner;
	OwnerComponent = InOwnerComponent;
	Priority = SourcePassive.Priority;
	bEnabled = SourcePassive.bEnabled;
}

void UCharacterPassive::BindingEvent(ABattleManager* BattleManager)
{
	if (CachedBattleManager)
	{
		CachedBattleManager->BattleActionStartDelegate.RemoveAll(this);
	}

	CachedBattleManager = BattleManager;
	if (CachedBattleManager)
	{
		CachedBattleManager->BattleActionStartDelegate.AddUObject(this, &UCharacterPassive::HandleBattleActionStart);
	}
}

void UCharacterPassive::NotifyBattleActionStart(const FBattleAction& BattleAction)
{
	HandleBattleActionStart(BattleAction);
}

void UCharacterPassive::Execute(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion)
{
	if (bExecutionActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CharacterPassive] Execution is already active."));
		CompleteExecution();
		return;
	}

	if (!bEnabled)
	{
		if (IsValid(OwnerComponent)) 
			OwnerComponent->NotifyPassiveExecutionFinished(this);
		return;
	}

	bExecutionActive = true;
	HandleExecution(OldPhase, NewPhase, bAllowDeferredCompletion);
}

void UCharacterPassive::CompleteExecution()
{
	if (!bExecutionActive)
	{
		return;
	}

	bExecutionActive = false;
	if (IsValid(OwnerComponent)) OwnerComponent->NotifyPassiveExecutionFinished(this);
}

void UCharacterPassive::HandleExecution(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion)
{
	static_cast<void>(bAllowDeferredCompletion);
	HandleBattlePhaseChanged(OldPhase, NewPhase);
	CompleteExecution();
}

void UCharacterPassive::HandleBattlePhaseChanged(EBattlePhase OldPhase,EBattlePhase NewPhase)
{
	// Override this function to handle battle phase changes.
	static_cast<void>(OldPhase);
	static_cast<void>(NewPhase);
}
