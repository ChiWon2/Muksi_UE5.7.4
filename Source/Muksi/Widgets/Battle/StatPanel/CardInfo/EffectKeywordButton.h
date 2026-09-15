// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SImage;
class STextBlock;

DECLARE_DELEGATE_OneParam(
	FOnEffectKeywordClicked,
	const FString&
);

DECLARE_DELEGATE_OneParam(
	FOnEffectKeywordHovered,
	const FString&
);

DECLARE_DELEGATE(
	FOnEffectKeywordUnhovered
);

class MUKSI_API SEffectKeywordButton : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEffectKeywordButton)
		: _ButtonStyle(nullptr)
		, _EffectId()
		, _DisplayText(FText::GetEmpty())
		, _IconBrush(nullptr)
		, _IconScale(1.0f)
	{
	}

		SLATE_ARGUMENT(const FButtonStyle*, ButtonStyle)
		SLATE_ARGUMENT(FString, EffectId)
		SLATE_ARGUMENT(FText, DisplayText)
		SLATE_ARGUMENT(const FSlateBrush*, IconBrush)
		SLATE_ARGUMENT(float, IconScale)

		SLATE_EVENT(FOnEffectKeywordClicked, OnEffectClicked)
		SLATE_EVENT(FOnEffectKeywordHovered, OnEffectHovered)
		SLATE_EVENT(FOnEffectKeywordUnhovered, OnEffectUnhovered)

SLATE_END_ARGS()

void Construct(const FArguments& InArgs);

private:
private:
	FReply HandleClicked();

	void HandleHovered();
	void HandleUnhovered();

	FString EffectId;
	
	float IconScale = 1.0f;

	FOnEffectKeywordClicked OnEffectClicked;
	FOnEffectKeywordHovered OnEffectHovered;
	FOnEffectKeywordUnhovered OnEffectUnhovered;

	FButtonStyle CachedButtonStyle;
	FSlateBrush CachedIconBrush;
	bool bHasIcon = false;
};
