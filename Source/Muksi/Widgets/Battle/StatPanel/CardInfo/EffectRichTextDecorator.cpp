// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextDecorator.h"

#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectKeywordButton.h"
#include "Muksi/Widgets/Battle/StatPanel/CardInfo/EffectRichTextBlock.h"


FEffectRichTextDecorator::FEffectRichTextDecorator(
	URichTextBlock* InOwner,
	const UEffectRichTextDecorator* InDecorator)
	: FRichTextDecorator(InOwner)
	, OwnerRichTextBlock(InOwner)
	, DecoratorObject(InDecorator)
{
}

bool FEffectRichTextDecorator::Supports(const FTextRunParseResults& RunParseResult, const FString& Text) const
{
	return RunParseResult.Name.Equals(
		TEXT("Effect"),
		ESearchCase::IgnoreCase
	);
}

TSharedPtr<SWidget>
FEffectRichTextDecorator::CreateDecoratorWidget(
	const FTextRunInfo& RunInfo,
	const FTextBlockStyle& DefaultTextStyle) const
{
	const FString* EffectId =
		RunInfo.MetaData.Find(TEXT("id"));

	if (!EffectId)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Effect 태그에 id가 없습니다.")
		);

		return SNew(STextBlock)
			.Text(RunInfo.Content)
			.TextStyle(&DefaultTextStyle);
	}
	
	

	const FString EffectIdString = *EffectId;
	const FName EffectName(*EffectId);

	const FEffectKeywordStyle* KeywordStyle = nullptr;
	float IconScale = 1.0f;

	if (const UEffectRichTextBlock* EffectRichTextBlock =
		Cast<UEffectRichTextBlock>(OwnerRichTextBlock.Get()))
	{
		KeywordStyle = EffectRichTextBlock->KeywordStyles.Find(EffectName);
		
		IconScale = EffectRichTextBlock->GetEffectIconScale();
	}

	TWeakObjectPtr<UEffectRichTextBlock> WeakEffectRichTextBlock =
		Cast<UEffectRichTextBlock>(OwnerRichTextBlock.Get());
	

	const FEffectKeywordStyle EffectStyleCopy =
		KeywordStyle
			? *KeywordStyle
			: FEffectKeywordStyle();

	return SNew(SEffectKeywordButton)
		.EffectId(EffectIdString)
		.DisplayText(RunInfo.Content)
		.IconBrush(
			KeywordStyle
				? &KeywordStyle->IconBrush
				: nullptr
		)
		.ButtonStyle(
			KeywordStyle
				? &KeywordStyle->ButtonStyle
				: nullptr
		)
		.IconScale(IconScale)
		.OnEffectClicked(
			FOnEffectKeywordClicked::CreateLambda(
				[
					WeakEffectRichTextBlock,
					EffectName,
					EffectStyleCopy
				](const FString& ClickedEffectId)
				{
					if (UEffectRichTextBlock* RichTextBlock =
						WeakEffectRichTextBlock.Get())
					{
						RichTextBlock->ShowEffectDescriptionByHover(
							EffectName,
							EffectStyleCopy
						);
					}
				}
			)
		).OnEffectHovered(
		FOnEffectKeywordHovered::CreateLambda(
			[
				WeakEffectRichTextBlock,
				EffectName,
				EffectStyleCopy
			](const FString&)
			{
				if (UEffectRichTextBlock* RichTextBlock =
					WeakEffectRichTextBlock.Get())
				{
					if (RichTextBlock->IsHoverPopupEnabled())
					{
						RichTextBlock->ShowEffectDescriptionByHover(
							EffectName,
							EffectStyleCopy
						);
					}
				}
			}
		)
	).OnEffectUnhovered(
		FOnEffectKeywordUnhovered::CreateLambda(
			[WeakEffectRichTextBlock]()
			{
				if (UEffectRichTextBlock* RichTextBlock =
					WeakEffectRichTextBlock.Get())
				{
					/*if (!RichTextBlock->IsHoverPopupEnabled())
					{
						return;
					}*/

					RichTextBlock->HideEffectDescriptionByHover();
				}
			}
		)
	);
}


TSharedPtr<ITextDecorator>
UEffectRichTextDecorator::CreateDecorator(
	URichTextBlock* InOwner)
{
	return MakeShared<FEffectRichTextDecorator>(
		InOwner,
		this
	);
}