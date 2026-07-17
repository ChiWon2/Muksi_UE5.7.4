// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EffectDescriptionPopup.generated.h"

class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class MUKSI_API UEffectDescriptionPopup : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetEffectData(
		const FText& EffectName,
		const FText& EffectDescription,
		const FSlateBrush& EffectIcon
	);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EffectIconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EffectDescriptionText;
};
