// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectKeywordButton.h"

#include "Styling/CoreStyle.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"

void SEffectKeywordButton::Construct(
	const FArguments& InArgs)
{
	EffectId = InArgs._EffectId;
	IconScale = InArgs._IconScale;
	
	OnEffectClicked = InArgs._OnEffectClicked;
	OnEffectHovered = InArgs._OnEffectHovered;
	OnEffectUnhovered = InArgs._OnEffectUnhovered;
	

	if (InArgs._ButtonStyle)
	{
		CachedButtonStyle = *InArgs._ButtonStyle;
	}
	else
	{
		CachedButtonStyle =
			FCoreStyle::Get()
			.GetWidgetStyle<FButtonStyle>(
				TEXT("NoBorder")
			);
	}
	
	if (InArgs._IconBrush)
	{
		CachedIconBrush = *InArgs._IconBrush;
		bHasIcon = true;
	}
	else
	{
		bHasIcon = false;
	}

	ChildSlot
	[
		SNew(SButton)
		.ButtonStyle(&CachedButtonStyle)
		.ContentPadding(FMargin(0.0f))
		.Cursor(EMouseCursor::Hand)
		.OnClicked(
			this,
			&SEffectKeywordButton::HandleClicked
		)
		.OnHovered(
	this,
			&SEffectKeywordButton::HandleHovered
		)
		.OnUnhovered(
			this,
			&SEffectKeywordButton::HandleUnhovered
		)
		[
			//버튼 위치 조절
			SNew(SBox)
	.MinDesiredHeight(50.0f)
	.VAlign(VAlign_Center)
	[
		SNew(SHorizontalBox)

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		.Padding(0.0f, 0.0f, 4.0f, 0.0f)
		[
			SNew(SBox)
			.WidthOverride(
				bHasIcon
					? CachedIconBrush.ImageSize.X * IconScale
					: 0.0f
			)
			.HeightOverride(
				bHasIcon
					? CachedIconBrush.ImageSize.Y * IconScale
					: 0.0f
			)
			.VAlign(VAlign_Center)
			.Visibility(
				bHasIcon
					? EVisibility::Visible
					: EVisibility::Collapsed
			)
			[
				SNew(SImage)
				.Image(bHasIcon ? &CachedIconBrush : nullptr)
			]
		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(InArgs._DisplayText)
		]
	]
			/*SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			.Padding(0.0f, 0.0f, 4.0f, 0.0f)
			[
				SNew(SBox)
					.WidthOverride(
						bHasIcon
							? CachedIconBrush.ImageSize.X * IconScale
							: 0.0f
					)
					.HeightOverride(
						bHasIcon
							? CachedIconBrush.ImageSize.Y * IconScale
							: 0.0f
					)
					.VAlign(VAlign_Center)
					.Visibility(
						bHasIcon
							? EVisibility::Visible
							: EVisibility::Collapsed
					)
					[
						SNew(SImage)
						.Image(bHasIcon ? &CachedIconBrush : nullptr)
					]
			]

			+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(InArgs._DisplayText)
			]*/
		]
	];
}

FReply SEffectKeywordButton::HandleClicked()
{
	if (OnEffectClicked.IsBound())
	{
		OnEffectClicked.Execute(EffectId);
	}

	return FReply::Handled();
}

void SEffectKeywordButton::HandleHovered()
{
	if (OnEffectHovered.IsBound())
	{
		OnEffectHovered.Execute(EffectId);
	}
}

void SEffectKeywordButton::HandleUnhovered()
{
	if (OnEffectUnhovered.IsBound())
	{
		OnEffectUnhovered.Execute();
	}
}
