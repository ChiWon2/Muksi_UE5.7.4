// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Passive/CharacterPassive.h"

#include "Muksi/Contents/Battle/Passive/CharacterPassiveComponent.h"

void UCharacterPassive::BeginDestroy()
{
	CompleteExecution();
	Super::BeginDestroy();
}

void UCharacterPassive::InitializePassive(ABattleCharacterBase* InOwner, UCharacterPassiveComponent* InOwnerComponent)
{
	OwnerCharacter = InOwner;
	OwnerComponent = InOwnerComponent;
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
