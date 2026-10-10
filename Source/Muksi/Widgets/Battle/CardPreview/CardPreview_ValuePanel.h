// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreview_ValuePanel.generated.h"

class UTextBlock;
class UImage;

UENUM()
enum class EValueType : uint8
{
	Attack,
};

/**
 * 
 */
UCLASS()
class MUKSI_API UCardPreview_ValuePanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetValue(EValueType Type, int32 Value);
	
protected:
	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Value;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_ValueType;
	
	UPROPERTY(EditDefaultsOnly, Category = "Battle|Preview")
	TMap<EValueType, TObjectPtr<UImage>> TypeStyles;
};
