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

	UBattleRuntimeContext* RuntimeContext = BattleManager->GetBattleRuntimeContext();

	if (!RuntimeContext)
	{
		return false;
	}

	return ResolveCharacterPanicOnTimeout(RuntimeContext->GetEnemyCharacter(), ExchangeIndex,OutResult);
}

bool UBattleCardManager::ResolveCharacterPanicOnTimeout(ABattleCharacterBase* Character, int32 ExchangeIndex,
	FCharacterPanicTimeoutResult& OutResult)
{
	OutResult = FCharacterPanicTimeoutResult();

    if (!IsValid(Character))
    {
        return false;
    }

    UBattleCardComponent* CardComponent = Character->GetBattleCardComponent();
    UMuksiCharacterDataAsset* CharacterData = Character->GetCharacterData();

    if (!CardComponent || !CharacterData)
    {
        return false;
    }

    //Commit 된 카드가 Panic 카드인지 확인
    const FBattleCardInstance* ExistingCard = CardComponent->GetCommittedCardByExchange(ExchangeIndex);

    if (ExistingCard)
    {
        if (ExistingCard->Source == EBattleCardInstanceSource::Panic)
        {
            OutResult.PanicCard = *ExistingCard; //Panic 카드면 그냥 쓰기
            return true;
        }
    }

    
//Panic 카드 배열 가져오기
    TArray<const FCharacterPanicData*> ValidPanicDataArray;

    for (const FCharacterPanicData& PanicData : CharacterData->TimeoutPenalties)
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
                "No valid Panic penalty cards. Character=%s"),
            *GetNameSafe(Character));

        return false;
    }

    
	//Panic 카드 선택
    const int32 RandomIndex = FMath::RandHelper(ValidPanicDataArray.Num());

    const FCharacterPanicData* SelectedPanicData = ValidPanicDataArray[RandomIndex];

    if (!SelectedPanicData)
    {
        return false;
    }
	

	//commit 된 일반 카드 있으면 되돌리기
    if (ExistingCard)
    {
        OutResult.ReturnedCard = *ExistingCard;
        OutResult.bReturnedCard = true;

        const FGuid ExistingCardId = ExistingCard->InstanceId;

        if (!CardComponent->ReturnCommittedCard(ExistingCardId))
        {
            return false;
        }
    }

    // 손패 랜덤 카드 한 장 버림
    OutResult.bDiscardedCard = CardComponent->DiscardRandomHandCard(OutResult.DiscardedCard);

    // 랜덤으로 선택된 Panic 카드 Commit
    if (!CardComponent->CommitPanicCard(SelectedPanicData->PenaltyCard,OutResult.PanicCard))
    {
        return false;
    }

    UE_LOG(
        LogTemp,
        Warning,
        TEXT(
            "[BattleCardManager] "
            "Panic timeout resolved. "
            "Character=%s PanicCard=%s DiscardedCard=%s"),
        *GetNameSafe(Character),
        *GetNameSafe(OutResult.PanicCard.CardData),
        OutResult.bDiscardedCard
            ? *GetNameSafe(OutResult.DiscardedCard.CardData)
            : TEXT("None"));

    return true;
}



