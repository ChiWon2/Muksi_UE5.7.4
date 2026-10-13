// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreview_ValuePanel.generated.h"

enum class EDescriptionValueType : uint8;
class UTextBlock;
class UImage;
class UTexture2D;

/**
 * 
 */
UCLASS()
class MUKSI_API UCardPreview_ValuePanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetValue(EDescriptionValueType ValueType, int32 Value);
	
protected:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Value;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ValueType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Battle|Preview")
	TMap<EDescriptionValueType, TObjectPtr<UTexture2D>> TypeStyles;
};
