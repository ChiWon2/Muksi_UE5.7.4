// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveInfoPanel.generated.h"

class URichTextBlockWidget;
class UCharacterPassive;

class UVerticalBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UPassiveInfoPanel : public UUserWidget
{
	GENERATED_BODY()
	
	
protected:
	//**BindWidget**//

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* EffectVerticalBox;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_PassiveName;
	
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Effect"
	)
	TSubclassOf<URichTextBlockWidget> RichTextBlockWidgetClass;
	
	
	
public:
	void SetPassiveData(UCharacterPassive* CharacterPassive);
};
