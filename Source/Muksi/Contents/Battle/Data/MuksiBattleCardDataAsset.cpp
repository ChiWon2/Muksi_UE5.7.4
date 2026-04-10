// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"

UMuksiBattleCardDataAsset::UMuksiBattleCardDataAsset()
{
	CardID = NAME_None;
	CardName = FText::FromString(TEXT("Default Card"));
	CardDescription = FText::FromString(TEXT("Card Description"));
	CardTexture = nullptr;
}
