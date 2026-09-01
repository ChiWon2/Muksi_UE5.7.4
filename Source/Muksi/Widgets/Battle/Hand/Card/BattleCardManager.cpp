// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Hand/Card/BattleCardManager.h"

#include "Muksi/Contents/Battle/BattleManager.h"
#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Enemy.h"
#include "Muksi/Contents/Battle/Character/BattleCharacter_Player.h"
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
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("[BattleCardManager] RoundEnd Entry"));
			HandleRoundEnd();
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
		if (UBattleCardComponent* CardComponent = PlayerCharacter->GetBattleCardComponent())
		{
			CardComponent->RefillHandIfEmpty();
		}
	}

	if (EnemyCharacter)
	{
		if (UBattleCardComponent* CardComponent = EnemyCharacter->GetBattleCardComponent())
		{
			CardComponent->RefillHandIfEmpty();
		}
	}
}

void UBattleCardManager::HandleRoundEnd()
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
		if (UBattleCardComponent* CardComponent = PlayerCharacter->GetBattleCardComponent())
		{
			CardComponent->ConsumeCommittedCards();
		}
	}

	if (EnemyCharacter)
	{
		if (UBattleCardComponent* CardComponent = EnemyCharacter->GetBattleCardComponent())
		{
			CardComponent->ConsumeCommittedCards();
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

	UBattleCardComponent* CardComponent =
		Character->GetBattleCardComponent();

	if (!IsValid(CardComponent))
	{
		return false;
	}

	if (!CardComponent->ReplaceHandCard(
		InstanceId,
		NewCardData))
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



bool UBattleCardManager::SelectRandomPlayerCardOnTimeout(FBattleCardInstance& OutSelectedCard)
{
	if (!BattleManager)
	{
		UE_LOG(LogTemp, Error, TEXT("BattleManager is null (BattleCardManager.cpp)"));
		return false;
	}
	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return false;
	}
	
	
	ABattleCharacterBase* PlayerCharacter = RuntimeContext->GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return false;
	}

	UBattleCardComponent* CardComponent = PlayerCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	const TArray<FBattleCardInstance>& CurrentHand = CardComponent->GetCurrentHand();

	if (CurrentHand.IsEmpty())
	{
		return false;
	}

	const int32 RandomIndex = FMath::RandRange(0, CurrentHand.Num() - 1);
	
	OutSelectedCard = CurrentHand[RandomIndex];

	if (!CardComponent->CommitCard(OutSelectedCard.InstanceId))
	{
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT("Timeout Random Card Selected: %s"), *GetNameSafe(OutSelectedCard.CardData));

	return true;
}

bool UBattleCardManager::ResolvePlayerCardOnTimeout(int32 ExchangeIndex, FBattleCardInstance& OutCard, bool& bOutAutoSelected)
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

	ABattleCharacterBase* PlayerCharacter = RuntimeContext->GetPlayerCharacter();

	if (!PlayerCharacter)
	{
		return false;
	}

	UBattleCardComponent* CardComponent = PlayerCharacter->GetBattleCardComponent();

	if (!CardComponent)
	{
		return false;
	}

	// 현재 Exchange에서 이미 Commit한 카드가 있으면 그대로 사용
	if (const FBattleCardInstance* CommittedCard = CardComponent->GetCommittedCardByExchange(ExchangeIndex))
	{
		OutCard = *CommittedCard;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Timeout - Use Existing Card: %s"),
			*GetNameSafe(OutCard.CardData)
		);

		return true;
	}

	// 없으면 랜덤 카드 Commit
	bOutAutoSelected = true;
	return SelectRandomPlayerCardOnTimeout(OutCard);
}


