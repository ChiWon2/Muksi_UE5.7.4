// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlockDecorator.h"
#include "EffectRichTextDecorator.generated.h"

class UEffectRichTextDecorator;

class FEffectRichTextDecorator : public FRichTextDecorator
{
public:
	FEffectRichTextDecorator(
		URichTextBlock* InOwner,
		const UEffectRichTextDecorator* InDecorator
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
	TWeakObjectPtr<URichTextBlock> OwnerRichTextBlock;
	const UEffectRichTextDecorator* DecoratorObject = nullptr;
};
/**
 * 
 */
UCLASS()
class MUKSI_API UEffectRichTextDecorator : public URichTextBlockDecorator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Style")
	FButtonStyle KeywordButtonStyle;
	
	virtual TSharedPtr<ITextDecorator> CreateDecorator(
		URichTextBlock* InOwner
	) override;
	

};


