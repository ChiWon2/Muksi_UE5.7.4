// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/Widget_DeckCardEntry.h"

#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"




void UWidget_DeckCardEntry::SetCardRowData(UDataTable* InCardDataTable, FName InCardRowName)
{
	CachedCardDataTable = InCardDataTable;
	CachedCardRowName = InCardRowName;

	RefreshCardData();
}

void UWidget_DeckCardEntry::SetCardDataAsset(UMuksiBattleCardDataAsset* InCardDataAsset)
{
	CachedCardData = InCardDataAsset;
	RefreshCardData();
}

void UWidget_DeckCardEntry::NativeConstruct()
{
	Super::NativeConstruct();
	RefreshCardData();
}

void UWidget_DeckCardEntry::RefreshCardData()
{
	if (!IsValid(CachedCardData))
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_DeckCardEntry::RefreshCardData - CachedCardDataAsset is invalid"));
		return;
	}

	if (CardNameText)
	{
		CardNameText->SetText(CachedCardData->CardName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UWidget_DeckCardEntry::RefreshCardData - CardNameText is null"));
	}

	if (CardDescriptionText)
	{
		CardDescriptionText->SetText(CachedCardData->CardDescription);
	}

	if (CardImage)
	{
		if (CachedCardData->CardTexture)
		{
			CardImage->SetBrushFromTexture(CachedCardData->CardTexture);
		}
		else
		{
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("UWidget_DeckCardEntry::RefreshCardData - CardTexture is null: %s"),
				*GetNameSafe(CachedCardData.Get())
			);
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT("UWidget_DeckCardEntry::RefreshCardData - Card Applied: %s"),
		*CachedCardData->CardName.ToString()
	);
}
