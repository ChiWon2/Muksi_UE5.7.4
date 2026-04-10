// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_DeckCardEntry.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Muksi/Contents/Battle/Data/MMuksiBattleCardTableRow.h"




void UWidget_DeckCardEntry::SetCardRowData(UDataTable* InCardDataTable, FName InCardRowName)
{
	CachedCardDataTable = InCardDataTable;
	CachedCardRowName = InCardRowName;

	RefreshCardData();
}

void UWidget_DeckCardEntry::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshCardData();
}

void UWidget_DeckCardEntry::RefreshCardData()
{
	if (!CachedCardDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckCardEntry::RefreshCardData - CachedCardDataTable is null"));
		return;
	}

	if (CachedCardRowName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckCardEntry::RefreshCardData - CachedCardRowName is None"));
		return;
	}

	static const FString ContextString(TEXT("UWidget_CharacterDeckCardEntry::RefreshCardData"));
	const FMMuksiBattleCardTableRow* CardRow =
		CachedCardDataTable->FindRow<FMMuksiBattleCardTableRow>(CachedCardRowName, ContextString);

	if (!CardRow)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckCardEntry::RefreshCardData - CardRow not found: %s"), *CachedCardRowName.ToString());
		return;
	}

	if (CardNameText)
	{
		CardNameText->SetText(CardRow->CardName);
	}
	

	if (CardImage && CardRow->CardTexture)
	{
		CardImage->SetBrushFromTexture(CardRow->CardTexture);
	}
}
