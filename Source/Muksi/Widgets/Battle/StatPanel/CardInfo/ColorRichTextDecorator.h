// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "ColorRichTextDecorator.generated.h"

class SWidget;
class UColorRichTextDecorator;

class FColorRichTextDecorator : public FRichTextDecorator
{
public:
	FColorRichTextDecorator(
		URichTextBlock* InOwner,
		const UColorRichTextDecorator* InDecorator
	);

protected:
	virtual bool Supports(
		const FTextRunParseResults& RunParseResult,
		const FString& Text
	) const override;

	virtual TSharedPtr<SWidget> CreateDecoratorWidget(
		const FTextRunInfo& RunInfo,
		const FTextBlockStyle& DefaultTextStyle
	) const override;

private:
	const UColorRichTextDecorator* DecoratorObject = nullptr;
};

UCLASS(Blueprintable)
class MUKSI_API UColorRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()

public:
	// 태그의 color 값과 연결되는 색상 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Text Style")
	TMap<FName, FLinearColor> TextColors;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Underline",
		meta = (ClampMin = "0.1"))
	float UnderlineThickness = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Underline")
	float UnderlineBottomPadding = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Underline")
	float UnderlineHorizontalPadding = 0.0f;

protected:
	virtual TSharedPtr<ITextDecorator> CreateDecorator(
		URichTextBlock* InOwner
	) override;
};