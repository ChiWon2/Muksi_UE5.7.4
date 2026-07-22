// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Widgets/Battle/StatPanel/CardInfo/ColorRichTextDecorator.h"

#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"

bool FColorRichTextDecorator::Supports(
	const FTextRunParseResults& RunParseResult,
	const FString& Text
) const
{
	return RunParseResult.Name.Equals(
		TEXT("Style"),
		ESearchCase::IgnoreCase
	);
}

TSharedPtr<SWidget> FColorRichTextDecorator::CreateDecoratorWidget(
	const FTextRunInfo& RunInfo,
	const FTextBlockStyle& DefaultTextStyle
) const
{
	FTextBlockStyle TextStyle = DefaultTextStyle;

	// color="Damage" 같은 값을 에디터의 TextColors TMap에서 찾음
	if (const FString* ColorValue = RunInfo.MetaData.Find(TEXT("color")))
	{
		if (DecoratorObject)
		{
			const FName ColorId(*ColorValue);

			if (const FLinearColor* FoundColor =
				DecoratorObject->TextColors.Find(ColorId))
			{
				TextStyle.SetColorAndOpacity(*FoundColor);
			}
		}
	}

	// bold="true"
	if (const FString* BoldValue = RunInfo.MetaData.Find(TEXT("bold")))
	{
		const bool bBold =
			BoldValue->Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
			BoldValue->Equals(TEXT("1"));

		if (bBold)
		{
			TextStyle.Font.TypefaceFontName = TEXT("Bold");
		}
	}

	// underline="true"
	const FString UnderlineValue =
		RunInfo.MetaData.FindRef(TEXT("underline"));

	const bool bUnderline =
		UnderlineValue.Equals(TEXT("true"), ESearchCase::IgnoreCase) ||
		UnderlineValue.Equals(TEXT("1"));

	// 에디터 설정값 사용. DecoratorObject가 없으면 기본값 사용
	const float UnderlineThickness =
		DecoratorObject
			? DecoratorObject->UnderlineThickness
			: 2.0f;

	const float UnderlineBottomPadding =
		DecoratorObject
			? DecoratorObject->UnderlineBottomPadding
			: 1.0f;

	const float UnderlineHorizontalPadding =
		DecoratorObject
			? DecoratorObject->UnderlineHorizontalPadding
			: 0.0f;

	return SNew(SOverlay)

		// 텍스트
		+ SOverlay::Slot()
		[
			SNew(STextBlock)
			.Text(RunInfo.Content)
			.Font(TextStyle.Font)
			.ColorAndOpacity(TextStyle.ColorAndOpacity)
			.ShadowOffset(TextStyle.ShadowOffset)
			.ShadowColorAndOpacity(TextStyle.ShadowColorAndOpacity)
		]

		// 밑줄
		+ SOverlay::Slot()
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(
			UnderlineHorizontalPadding,
			0.0f,
			UnderlineHorizontalPadding,
			UnderlineBottomPadding
		))
		[
			SNew(SBox)
			.HeightOverride(UnderlineThickness)
			.Visibility(
				bUnderline
					? EVisibility::HitTestInvisible
					: EVisibility::Collapsed
			)
			[
				SNew(SImage)
				.Image(&DefaultTextStyle.UnderlineBrush)
				.ColorAndOpacity(TextStyle.ColorAndOpacity)
			]
		];
}

FColorRichTextDecorator::FColorRichTextDecorator(
	URichTextBlock* InOwner,
	const UColorRichTextDecorator* InDecorator
)
	: FRichTextDecorator(InOwner)
	, DecoratorObject(InDecorator)
{
}

TSharedPtr<ITextDecorator> UColorRichTextDecorator::CreateDecorator(
	URichTextBlock* InOwner
)
{
	return MakeShared<FColorRichTextDecorator>(
		InOwner,
		this
	);
}