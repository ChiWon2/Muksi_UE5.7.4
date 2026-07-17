// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardInfoPanel.generated.h"

class USizeBox;
class UVerticalBox;
class UTextBlock;

class UMuksiBattleCardDataAsset;
class UWidget_DeckCardEntry;
class UEffectRichTextBlock;
class URichTextBlockWidget;

/**
 * 
 */
UCLASS()
class MUKSI_API UCardInfoPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//**BindWidget**//
	//카드 이미지
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_CardImage;
	
	//카드 설명 텍스트 블록
	
protected:
	//**BindWidget**//
	UPROPERTY(meta = (BindWidget))
	UWidget_DeckCardEntry* CardEntry;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* EffectVerticalBox;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock_CardName;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Effect"
	)
	TSubclassOf<URichTextBlockWidget> RichTextBlockWidgetClass;
	
public:
	void SetCardData(UMuksiBattleCardDataAsset* CardData);
	
};
