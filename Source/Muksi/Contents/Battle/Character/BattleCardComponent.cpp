// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/BattleCardComponent.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

// Sets default values for this component's properties
UBattleCardComponent::UBattleCardComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBattleCardComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBattleCardComponent::Initialize(const TArray<UMuksiBattleCardDataAsset*>& InDeck)
{
	FullDeck.Empty();
	for (UMuksiBattleCardDataAsset* BattleCard : InDeck)
	{
		FullDeck.Add(BattleCard);
	}

	CurrentHand.Empty();
	CommittedCards.Empty();

	for (UMuksiBattleCardDataAsset* CardData : FullDeck)
	{
		if (!CardData)
		{
			continue;
		}

		CurrentHand.Emplace(CardData);
	}
}

const FBattleCardInstance* UBattleCardComponent::FindHandCardById(const FGuid& InstanceId) const
{
	return CurrentHand.FindByPredicate(
		[&InstanceId](const FBattleCardInstance& CardInstance)
		{
			return CardInstance.InstanceId == InstanceId;
		});
}

const FBattleCardInstance* UBattleCardComponent::FindHandCardByData(UMuksiBattleCardDataAsset* CardData) const
{
	if (!IsValid(CardData))
	{
		return nullptr;
	}

	return CurrentHand.FindByPredicate(
		[CardData](const FBattleCardInstance& CardInstance)
		{
			return CardInstance.CardData == CardData;
		});
}

const FBattleCardInstance* UBattleCardComponent::GetCommittedCardByExchange(int32 ExchangeIndex) const
{
	if (!CommittedCards.IsValidIndex(ExchangeIndex))
	{
		return nullptr;
	}

	return &CommittedCards[ExchangeIndex];
}


//카드 이동이 성공했는지 실패했는지 호출한 쪽에서 알 수 있게 하기
bool UBattleCardComponent::CommitCard(const FGuid& InstanceId)
{
	const int32 CardIndex = CurrentHand.IndexOfByPredicate(
		[&InstanceId](const FBattleCardInstance& CardInstance)
		{
			return CardInstance.InstanceId == InstanceId;
		});

	if (CardIndex == INDEX_NONE)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[%s] CommitCard Failed - Card not found"),
			*GetNameSafe(GetOwner()));
		
		return false;
	}

	FBattleCardInstance CardInstance = CurrentHand[CardIndex];

	CurrentHand.RemoveAt(CardIndex);
	CommittedCards.Add(CardInstance);
	
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[%s] CommitCard Success - Hand: %d / Committed: %d (BattleCardComponent.cpp)"),
		*GetNameSafe(GetOwner()),
		CurrentHand.Num(),
		CommittedCards.Num());

	return true;
}

//ExchangeSlot에 올린 카드를 다시 취소시 여기 데이터에서도 다시 원래대로 되돌려 놓기
bool UBattleCardComponent::ReturnCommittedCard(const FGuid& InstanceId)
{
	const int32 CardIndex = CommittedCards.IndexOfByPredicate(
		[&InstanceId](const FBattleCardInstance& CardInstance)
		{
			return CardInstance.InstanceId == InstanceId;
		});

	if (CardIndex == INDEX_NONE)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[%s] ReturnCommittedCard Failed - Card not found"),
			*GetNameSafe(GetOwner()));
		
		return false;
	}
	
	FBattleCardInstance CardInstance = CommittedCards[CardIndex];
	
	//패닉 카드는 반환 금지
	if (CardInstance.Source ==
	EBattleCardInstanceSource::Panic)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"[%s] Panic card cannot return to hand: %s"),
			*GetNameSafe(GetOwner()),
			*GetNameSafe(CardInstance.CardData));

		return false;
	}

	CommittedCards.RemoveAt(CardIndex);
	CurrentHand.Add(CardInstance);
	
	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[%s] ReturnCommittedCard Success - Hand: %d / Committed: %d (BattleCardComponent.cpp)"),
		*GetNameSafe(GetOwner()),
		CurrentHand.Num(),
		CommittedCards.Num());

	return true;
}

//특정 카드를 NewCardData로 변경
bool UBattleCardComponent::ReplaceHandCard(const FGuid& InstanceId, UMuksiBattleCardDataAsset* NewCardData)
{
	if (!NewCardData)
	{
		return false;
	}

	FBattleCardInstance* CardInstance =
		CurrentHand.FindByPredicate(
			[&InstanceId](const FBattleCardInstance& Instance)
			{
				return Instance.InstanceId == InstanceId;
			});

	if (!CardInstance)
	{
		return false;
	}
	
	CardInstance->CardData = NewCardData;
	return true;
}

void UBattleCardComponent::ConsumeCommittedCards()
{
	CommittedCards.Empty();
	/*UE_LOG(
		LogTemp,
		Warning,
		TEXT("[%s] ConsumeCommittedCards - Hand: %d / Committed: %d (BattleCardComponent.cpp)"),
		*GetNameSafe(GetOwner()),
		CurrentHand.Num(),
		CommittedCards.Num());*/
}

bool UBattleCardComponent::RefillHandIfEmpty()
{
	if (CurrentHand.Num() > 0 || CommittedCards.Num() > 0)
	{
		return false;
	}

	for (UMuksiBattleCardDataAsset* CardData : FullDeck)
	{
		if (!CardData)
		{
			continue;
		}

		CurrentHand.Emplace(CardData);
	}

	return CurrentHand.Num() > 0;
}

bool UBattleCardComponent::DiscardRandomHandCard(FBattleCardInstance& OutDiscardedCard)
{
	if (CurrentHand.IsEmpty())
	{
		return false;
	}

	const int32 RandomIndex =
		FMath::RandRange(
			0,
			CurrentHand.Num() - 1);

	OutDiscardedCard =
		CurrentHand[RandomIndex];

	CurrentHand.RemoveAt(RandomIndex);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[%s] Random hand card discarded: %s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(OutDiscardedCard.CardData));

	return true;
}

bool UBattleCardComponent::CommitPanicCard(UMuksiBattleCardDataAsset* PanicCard, FBattleCardInstance& OutCommittedCard)
{
	if (!IsValid(PanicCard))
	{
		return false;
	}

	OutCommittedCard = FBattleCardInstance(PanicCard, EBattleCardInstanceSource::Panic);

	CommittedCards.Add(OutCommittedCard);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"[%s] Panic card committed: %s"),
		*GetNameSafe(GetOwner()),
		*GetNameSafe(PanicCard));

	return true;
}



