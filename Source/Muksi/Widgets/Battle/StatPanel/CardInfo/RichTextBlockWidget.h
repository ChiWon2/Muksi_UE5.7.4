// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RichTextBlockWidget.generated.h"

class UEffectRichTextBlock;

/**
 * 
 */
UCLASS()
class MUKSI_API URichTextBlockWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetText(FText Text);
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEffectRichTextBlock> RichTextBlock;
};
