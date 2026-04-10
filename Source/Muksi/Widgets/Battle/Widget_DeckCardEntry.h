// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_DeckCardEntry.generated.h"

class UImage;
class UMuksiBattleCardDataAsset;
class UTextBlock;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_DeckCardEntry : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Deck Card Entry")
	void SetCardRowData(UDataTable* InCardDataTable, FName InCardRowName);

protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface
	
	
	void RefreshCardData();

protected:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CachedCardData;
	
	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CachedCardDataTable = nullptr;

	UPROPERTY(Transient)
	FName CachedCardRowName = NAME_None;
	
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> CardImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CardNameText;
	
	//***** BindWidget *****
};
