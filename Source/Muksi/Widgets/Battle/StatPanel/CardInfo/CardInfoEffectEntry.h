// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardInfoEffectEntry.generated.h"

struct FCardEffectDisplayData;
class UTextBlock;
class UEffectRichTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UCardInfoEffectEntry : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetEffectData(const FCardEffectDisplayData& InEffectData);
	
protected:
	//BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AttackValue;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEffectRichTextBlock> Text_EffectDescription;
};
