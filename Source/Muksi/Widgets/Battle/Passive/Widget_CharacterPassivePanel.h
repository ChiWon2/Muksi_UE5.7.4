// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CharacterPassivePanel.generated.h"

class UCharacterPassive;
class UPassiveInfoPanel;
class UScrollBox;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CharacterPassivePanel : public UUserWidget
{
	GENERATED_BODY()
public:
	/*UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void SetDeckData(UDataTable* InCardDataTable, const TArray<FName>& InDeckCardRowNames);*/
	
	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void SetPassiveData(TArray<UCharacterPassive*> InPassiveArray);
	
	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void RefreshPassivePanel();

	UFUNCTION(BlueprintCallable, Category = "Character Deck Panel")
	void ClearPassivePanel();
	
protected:
	// ***** BindWidget *****
	/*UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> DeckWrapBox;*/
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> PassiveScrollBox;
	// ***** BindWidget *****
	
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Deck Panel")
	TSubclassOf<UPassiveInfoPanel> PassiveInfoPanelClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Deck Panel")
	TArray<TObjectPtr<UPassiveInfoPanel>> PassiveInfoPanelArray;
	

	
	
};
