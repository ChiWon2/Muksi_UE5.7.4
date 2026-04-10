// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/Widget_CharacterDeckPanel.h"

#include "Muksi/Widgets/Battle/Widget_DeckCardEntry.h"
#include "Components/WrapBox.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"



void UWidget_CharacterDeckPanel::SetDeckData(UDataTable* InCardDataTable, const TArray<FName>& InDeckCardRowNames)
{
	CachedCardDataTable = InCardDataTable;
	CachedDeckCardRowNames = InDeckCardRowNames;

	RefreshDeckPanel();
}

void UWidget_CharacterDeckPanel::RefreshDeckPanel()
{
	ClearDeckPanel();

	if (!DeckWrapBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckPanel::RefreshDeckPanel - DeckWrapBox is null"));
		return;
	}

	if (!DeckCardEntryClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckPanel::RefreshDeckPanel - DeckCardEntryClass is null"));
		return;
	}

	if (!CachedCardDataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckPanel::RefreshDeckPanel - CachedCardDataTable is null"));
		return;
	}

	for (const FName& CardRowName : CachedDeckCardRowNames)
	{
		UWidget_DeckCardEntry* CardEntry =
			CreateWidget<UWidget_DeckCardEntry>(GetOwningPlayer(), DeckCardEntryClass);

		if (!CardEntry)
		{
			continue;
		}

		CardEntry->SetCardRowData(CachedCardDataTable, CardRowName);
		FString Text = CardRowName.ToString();
		UE_LOG(LogTemp, Log, TEXT("CachedCardDataTable %s"), *Text);
		DeckWrapBox->AddChildToWrapBox(CardEntry);
	}
}


void UWidget_CharacterDeckPanel::ClearDeckPanel()
{
	if (DeckWrapBox)
	{
		DeckWrapBox->ClearChildren();
	}
}

void UWidget_CharacterDeckPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
}
