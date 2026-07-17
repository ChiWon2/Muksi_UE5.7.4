// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/Widget_CharacterDeckPanel.h"

#include "Components/ScrollBox.h"
#include "Muksi/Widgets/Battle/Widget_DeckCardEntry.h"
#include "Components/WrapBox.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/CardInfoPanel.h"


/*void UWidget_CharacterDeckPanel::SetDeckData(UDataTable* InCardDataTable, const TArray<FName>& InDeckCardRowNames)
{
	CachedCardDataTable = InCardDataTable;
	CachedDeckCardRowNames = InDeckCardRowNames;

	RefreshDeckPanel();
}*/

void UWidget_CharacterDeckPanel::SetDeckData(TArray<UMuksiBattleCardDataAsset*> InCardDataArray)
{
	CachedCardDataArray.Reset();
	CardInfoPanelArray.Empty();

	for (UMuksiBattleCardDataAsset* CardData : InCardDataArray)
	{
		
		if (IsValid(CardData))
		{
			CachedCardDataArray.Add(CardData);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SetDeckDataFromArray - Invalid CardData"));
		}
		UCardInfoPanel* CardInfoPanel = CreateWidget<UCardInfoPanel>(GetOwningPlayer(), CardInfoPanelClass);
		CardInfoPanel->SetCardData(CardData);
		CardInfoPanelArray.Add(CardInfoPanel);
	}

	RefreshDeckPanelFromArray();
}

void UWidget_CharacterDeckPanel::RefreshDeckPanelFromArray()
{
	ClearDeckPanel();

	if (!DeckScrollBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefreshDeckPanelFromArray - DeckWrapBox is null"));
		return;
	}

	if (!DeckCardEntryClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("RefreshDeckPanelFromArray - DeckCardEntryClass is null"));
		return;
	}
	
	for (UCardInfoPanel* CardInfoPanel : CardInfoPanelArray)
	{
		if (!CardInfoPanel)continue;
		//DeckWrapBox->AddChildToWrapBox(CardInfoPanel);
		DeckScrollBox->AddChild(CardInfoPanel);
	}

	/*for (UMuksiBattleCardDataAsset* CardData : CachedCardDataArray)
	{
		if (!IsValid(CardData))
		{
			continue;
		}

		UWidget_DeckCardEntry* CardEntry =
			CreateWidget<UWidget_DeckCardEntry>(GetOwningPlayer(), DeckCardEntryClass);

		if (!CardEntry)
		{
			UE_LOG(LogTemp, Warning, TEXT("RefreshDeckPanelFromArray - Failed to create CardEntry"));
			continue;
		}

		CardEntry->SetCardDataAsset(CardData);

		DeckWrapBox->AddChildToWrapBox(CardEntry);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("Created Card Entry: %s"),
			*CardData->CardName.ToString()
		);
	}*/
}

void UWidget_CharacterDeckPanel::RefreshDeckPanel()
{
	ClearDeckPanel();

	if (!DeckScrollBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_CharacterDeckPanel::RefreshDeckPanel - DeckScrollBox is null"));
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
		DeckScrollBox->AddChild(CardEntry);
		//DeckWrapBox->AddChildToWrapBox(CardEntry);
	}
}


void UWidget_CharacterDeckPanel::ClearDeckPanel()
{
	if (DeckScrollBox)
	{
		DeckScrollBox->ClearChildren();
	}
}

void UWidget_CharacterDeckPanel::NativeConstruct()
{
	Super::NativeConstruct();
	
}
