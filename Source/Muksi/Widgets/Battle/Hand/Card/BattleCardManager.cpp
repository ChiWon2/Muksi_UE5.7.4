// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Hand/Card/BattleCardManager.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
#include "Muksi/Contents/Battle/Character/BattleSkillComponent.h"
#include "Muksi/Contents/Battle/Flow/BattlePhaseTask.h"
#include "Muksi/Contents/Battle/Runtime/BattleRuntimeContext.h"

bool UBattleCardManager::InitializeBattleFlow(ABattleManager* InBattleManager)
{
	if (!IsValid(InBattleManager))
	{
		return false;
	}

	BattleManager = InBattleManager;

	BattleManager->PhaseEntryRequestedDelegate.AddUniqueDynamic(this, &UBattleCardManager::HandlePhaseEntryRequested);

	return true;
}

void UBattleCardManager::Shutdown()
{
	if (BattleManager)
	{
		BattleManager->PhaseEntryRequestedDelegate.RemoveDynamic(
			this,
			&UBattleCardManager::HandlePhaseEntryRequested);
	}

	
	BattleManager = nullptr;
}

void UBattleCardManager::HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase,
	UBattlePhaseTaskContext* TaskContext)
{
	if (!TaskContext)
	{
		return;
	}

	if (NewPhase != EBattlePhase::RoundStart && NewPhase != EBattlePhase::RoundEnd)
	{
		return;
	}

	UBattlePhaseTask* PhaseTask = TaskContext->RegisterTask(this);

	if (!PhaseTask)
	{
		return;
	}

	switch (NewPhase)
	{
	case EBattlePhase::RoundStart:
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[BattleCardManager] RoundStart Entry"));
			
			HandleRoundStart();
			break;
		}

	case EBattlePhase::RoundEnd:
		{
			break;
		}

	default:
		break;
	}

	PhaseTask->Complete();
}

void UBattleCardManager::HandleRoundStart()
{
	if (!BattleManager)
	{
		return;
	}

	UBattleRuntimeContext* BattleRuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!BattleRuntimeContext)
	{
		return;
	}

	ABattleCharacterBase* PlayerCharacter = BattleRuntimeContext->GetPlayerCharacter();

	ABattleCharacterBase* EnemyCharacter = BattleRuntimeContext->GetEnemyCharacter();

	if (PlayerCharacter)
	{
		if (UBattleSkillComponent* SkillComponent = PlayerCharacter->GetBattleSkillComponent())
		{
			SkillComponent->ReduceCooldowns();
			SkillComponent->RestoreSkillCost();
		}
	}

	if (EnemyCharacter)
	{
		if (UBattleSkillComponent* SkillComponent = EnemyCharacter->GetBattleSkillComponent())
		{
			SkillComponent->ReduceCooldowns();
			SkillComponent->RestoreSkillCost();
		}
	}
}



bool UBattleCardManager::ReplaceHandCard(ABattleCharacterBase* Character, const FGuid& InstanceId,
	UMuksiBattleCardDataAsset* NewCardData)
{
	if (!IsValid(Character) || !IsValid(NewCardData))
	{
		return false;
	}
	

	ABattleCharacterBase* PlayerCharacter =
	BattleManager->GetBattleRuntimeContext()->GetPlayerCharacter();

	if (PlayerCharacter == Character)
	{
		OnBattleHandCardChanged.Broadcast(InstanceId, NewCardData);
	}
	
	return true;
}

bool UBattleCardManager::ResolvePlayerPanicOnTimeout(int32 ExchangeIndex, FCharacterPanicTimeoutResult& OutResult)
{
	if (!BattleManager)
	{
		return false;
	}

	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return false;
	}

	return ResolveCharacterPanicOnTimeout(RuntimeContext->GetPlayerCharacter(), ExchangeIndex,OutResult);
}

bool UBattleCardManager::ResolveEnemyPanicOnTimeout(int32 ExchangeIndex, FCharacterPanicTimeoutResult& OutResult)
{
	if (!BattleManager)
	{
		return false;
	}

	UBattleRuntimeContext* RuntimeContext =
		BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return false;
	}

	ABattleCharacterBase* EnemyCharacter =
		RuntimeContext->GetEnemyCharacter();

	if (!EnemyCharacter)
	{
		return false;
	}

	return ResolveCharacterPanicOnTimeout(
		EnemyCharacter,
		ExchangeIndex,
		OutResult);
}


bool UBattleCardManager::ResolveCharacterPanicOnTimeout(ABattleCharacterBase* Character, int32 ExchangeIndex,
                                                        FCharacterPanicTimeoutResult& OutResult)
{
	OutResult = FCharacterPanicTimeoutResult();

	if (!IsValid(Character))
	{
		return false;
	}

	UMuksiCharacterDataAsset* CharacterData =
		Character->GetCharacterData();

	if (!CharacterData)
	{
		return false;
	}

	TArray<const FCharacterPanicData*> ValidPanicDataArray;

	for (const FCharacterPanicData& PanicData :
		 CharacterData->TimeoutPenalties)
	{
		if (PanicData.IsValid())
		{
			ValidPanicDataArray.Add(&PanicData);
		}
	}

	if (ValidPanicDataArray.IsEmpty())
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[BattleCardManager] "
				"No valid Panic data. Character=%s"),
			*GetNameSafe(Character)
		);

		return false;
	}

	const int32 RandomIndex =
		FMath::RandHelper(ValidPanicDataArray.Num());

	const FCharacterPanicData* SelectedPanicData =
		ValidPanicDataArray[RandomIndex];

	if (!SelectedPanicData ||
		!SelectedPanicData->PenaltyCard)
	{
		return false;
	}

	OutResult.PanicCard = SelectedPanicData->PenaltyCard;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[BattleCardManager] "
			"Panic selected. Character=%s Panic=%s"),
		*GetNameSafe(Character),
		*GetNameSafe(OutResult.PanicCard)
	);

	return true;
}



