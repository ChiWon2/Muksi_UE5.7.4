// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveActivePopup.generated.h"

class UImage;
class UTextBlock;

/**
 * 
 */
UCLASS()
class MUKSI_API UPassiveActivePopup : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetData(UTexture2D* CharacterImage, FText PassiveName);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UImage* Image_CharacterImage;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* TextBlock_PassiveName;
	
};
