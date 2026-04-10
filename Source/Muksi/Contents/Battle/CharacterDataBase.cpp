// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/CharacterDataBase.h"

#include "Muksi/Contents/Battle/Data/MuksiCharacterDataAsset.h"


void UCharacterDataBase::InitializeFromDataAsset(UMuksiCharacterDataAsset* InCharacterAsset,
	UDataTable* InCardDataTable)
{
	SourceCharacterAsset = InCharacterAsset;
	CardDataTable = InCardDataTable;

	if (!SourceCharacterAsset || !CardDataTable)
	{
		CurrentHP = 0;
		DeckCardRowNames.Empty();
		return;
	}

	CurrentHP = SourceCharacterAsset->MaxHP;
	DeckCardRowNames = SourceCharacterAsset->StartingDeckCardRowNames;
}

const FMMuksiBattleCardTableRow* UCharacterDataBase::FindCardRow(FName InRowName) const
{
	if (!CardDataTable)
	{
		return nullptr;
	}

	static const FString ContextString(TEXT("UCharacterDataBase::FindCardRow"));
	return CardDataTable->FindRow<FMMuksiBattleCardTableRow>(InRowName, ContextString);
}
