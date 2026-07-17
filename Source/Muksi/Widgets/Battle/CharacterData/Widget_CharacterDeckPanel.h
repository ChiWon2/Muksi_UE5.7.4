// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CharacterDeckPanel.generated.h"

class UWidget_DeckCardEntry;
class UWrapBox;
class UMuksiBattleCardDataAsset;
class UCardInfoPanel;
class UScrollBox;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CharacterDeckPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	/*UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void SetDeckData(UDataTable* InCardDataTable, const TArray<FName>& InDeckCardRowNames);*/
	
	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void SetDeckData(TArray<UMuksiBattleCardDataAsset*> InCardDataArray);
	
	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void RefreshDeckPanelFromArray();

	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void RefreshDeckPanel();

	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void ClearDeckPanel();
	
protected:
	//~ Begin UUserWidget Interface
	virtual void NativeConstruct() override;
	//~ End UUserWidget Interface
protected:
	// ***** BindWidget *****
	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> DeckWrapBox;*/
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> DeckScrollBox;
	// ***** BindWidget *****
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Deck Panel")
	TSubclassOf<UWidget_DeckCardEntry> DeckCardEntryClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Deck Panel")
	TSubclassOf<UCardInfoPanel> CardInfoPanelClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Deck Panel")
	TArray<TObjectPtr<UCardInfoPanel>> CardInfoPanelArray;

	UPROPERTY(Transient)
	TObjectPtr<UDataTable> CachedCardDataTable = nullptr;

	UPROPERTY(Transient)
	TArray<FName> CachedDeckCardRowNames;
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> CachedCardDataArray;
};
