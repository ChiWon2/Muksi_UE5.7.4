// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/CharacterDataBase.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"


void UCharacterDataBase::InitializeFromDataAsset(UMuksiCharacterDataAsset* InCharacterAsset
	)
{
	//캐릭터 관련 스탯 최신화 하기
	
	SourceCharacterAsset = InCharacterAsset;
	
	for (UMuksiBattleCardDataAsset* CardData : SourceCharacterAsset->CharacterDeck)
	{
		if (!CardData)
		{
			continue;
		}

		BattleDeck.Add(CardData);
	}
	//HP
	MaxHP = SourceCharacterAsset->MaxHP;
	CurrentHP = SourceCharacterAsset->MaxHP;
	
	CreateBattleDeck(SourceCharacterAsset->CharacterDeck);
	//DeckCardRowNames = SourceCharacterAsset->StartingDeckCardRowNames;
}




void UCharacterDataBase::CreateBattleDeck(TArray<UMuksiBattleCardDataAsset*> InBattleDeckData)
{
	BattleDeckData = InBattleDeckData;
	BattleDeck.Empty();
	for (UMuksiBattleCardDataAsset* CardData : BattleDeckData)
	{
		if (!CardData)
		{
			continue;
		}

		BattleDeck.Add(CardData);
	}
}

TArray<UMuksiBattleCardDataAsset*> UCharacterDataBase::GetCharacterDeck()
{
	if (BattleDeck.Num() > 0){return BattleDeck;}
	
	if (BattleDeckData.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefillBattleDeckIfEmpty failed: CharacterDeck is empty"));
		return BattleDeck;
	}
	
	//Reroll
	for (UMuksiBattleCardDataAsset* CardData : BattleDeckData)
	{
		if (!CardData)
		{
			continue;
		}

		BattleDeck.Add(CardData);
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("BattleDeck refilled from CharacterDataAsset. Count=%d"),
		BattleDeck.Num()
	);
	
	return BattleDeck;
}

bool UCharacterDataBase::RemoveCard(UMuksiBattleCardDataAsset* CardData)
{
	if (!CardData)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveCardFromBattleDeck failed: CardData is null"));
		return false;
	}

	for (int32 i = 0; i < BattleDeck.Num(); i++)
	{
		if (BattleDeck[i] == CardData)
		{
			BattleDeck.RemoveAt(i);

			UE_LOG(
				LogTemp,
				Log,
				TEXT("RemoveCardFromBattleDeck: Removed %s / Remain=%d"),
				*GetNameSafe(CardData),
				BattleDeck.Num()
			);

			return true;
		}
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("RemoveCardFromBattleDeck failed: Card not found - %s"),
		*GetNameSafe(CardData)
	);

	return false;
}
