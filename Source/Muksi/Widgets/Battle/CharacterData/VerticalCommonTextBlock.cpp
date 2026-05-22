// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/CharacterData/VerticalCommonTextBlock.h"

UVerticalCommonTextBlock::UVerticalCommonTextBlock()
{
	SourceText = FText::FromString(TEXT("세로"));
}

void UVerticalCommonTextBlock::SetVerticalText(const FText& InText)
{
	SourceText = InText;
	SetText(ConvertToVerticalText(SourceText));
}

void UVerticalCommonTextBlock::SynchronizeProperties()
{
	Super::SynchronizeProperties();
	SetText(ConvertToVerticalText(SourceText));
}

FText UVerticalCommonTextBlock::ConvertToVerticalText(const FText& InText) const
{
	const FString SourceString = InText.ToString();

	FString Result;

	for (int32 i = 0; i < SourceString.Len(); ++i)
	{
		Result.AppendChar(SourceString[i]);

		if (i < SourceString.Len() - 1)
		{
			Result.AppendChar(TEXT('\n'));

			if (bAddEmptyLineBetweenCharacters)
			{
				Result.AppendChar(TEXT('\n'));
			}
		}
	}

	return FText::FromString(Result);
}
