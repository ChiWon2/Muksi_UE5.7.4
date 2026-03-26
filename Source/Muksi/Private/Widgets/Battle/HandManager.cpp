// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Battle/HandManager.h"

#include "Widgets/Battle/CardObject.h"

void UHandManager::Initialize()
{
	HandCards.Empty();
}

void UHandManager::DrawInitialHands(int32 InDrawCount)
{
	HandCards.Empty();
	for (int32 i = 0; i < InDrawCount; i++)
	{
		UCardObject* NewCard = NewObject<UCardObject>(this);
		if (!NewCard){continue;}
		
		const FName CardID = FName(*FString::Printf(TEXT("Card_%d"), i + 1));
		const FText CardName = FText::FromString(FString::Printf(TEXT("Card %d"), i + 1));
		
		NewCard->InitCard(CardID, CardName);
		HandCards.Add(NewCard);
	}
}
