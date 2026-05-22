// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_PlayerProfilePanel.generated.h"

class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_PlayerProfilePanel : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	void SetImage();
	void SetIntroductionText();
protected:
	//***** BindWidget *****	Begin
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerImage;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerName;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerIntroduction;
	//***** BindWidget *****	End
	
};
