// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"

#include "Muksi/Contents/Battle/CharacterDataBase.h"

UMuksiBattleCardDataAsset* UEnemyCardSelectStrategyBase::SelectCardsForExchange_Implementation(UCharacterDataBase* EnemyData)
{

	if (!EnemyData)
	{
		return nullptr;
	}
	TArray<UMuksiBattleCardDataAsset*> Deck = EnemyData->GetCharacterDeck();

	if (Deck.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy deck is empty"));
		return nullptr;
	}

	UMuksiBattleCardDataAsset* SelectedCard = nullptr;

	for (UMuksiBattleCardDataAsset* Card : Deck)
	{
		if (!Card)
		{
			continue;
		}

		SelectedCard = Card;
		break;
	}

	if (!SelectedCard)
	{
		return nullptr;
	}

	EnemyData->RemoveCard(SelectedCard);
	return SelectedCard;
	
}
