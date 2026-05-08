// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/CharacterDataBase.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"


void UCharacterDataBase::InitializeFromDataAsset(UMuksiCharacterDataAsset* InCharacterAsset
	)
{
	SourceCharacterAsset = InCharacterAsset;
	
	for (UMuksiBattleCardDataAsset* CardData : SourceCharacterAsset->CharacterDeck)
	{
		if (!CardData)
		{
			continue;
		}

		BattleDeck.Add(CardData);
	}
	
	CurrentHP = SourceCharacterAsset->MaxHP;
	//DeckCardRowNames = SourceCharacterAsset->StartingDeckCardRowNames;
}



void UCharacterDataBase::ApplyDamage(int32 InDamage)
{
	CurrentHP = FMath::Max(CurrentHP - InDamage, 0);

	UE_LOG(LogTemp, Log, TEXT("%s took %d damage. Current HP: %d"),
		SourceCharacterAsset ? *SourceCharacterAsset->CharacterName.ToString() : TEXT("Unknown"),
		InDamage,
		CurrentHP);
}

TArray<UMuksiBattleCardDataAsset*> UCharacterDataBase::GetCharacterDeck()
{
	if (BattleDeck.Num() > 0){return BattleDeck;}
	
	if (!CharacterDataAsset){
		UE_LOG(LogTemp, Warning, TEXT("RefillBattleDeckIfEmpty failed: CharacterDataAsset is null"));
		return BattleDeck;
	}
	if (CharacterDataAsset->CharacterDeck.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefillBattleDeckIfEmpty failed: CharacterDeck is empty"));
		return BattleDeck;
	}
	
	//Reroll
	for (UMuksiBattleCardDataAsset* CardData : CharacterDataAsset->CharacterDeck)
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
