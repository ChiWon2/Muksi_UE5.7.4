// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreviewEffectPanel.generated.h"

class UVerticalBox;

class UMuksiBattleCardDataAsset;
class UCardPreviewEffectEntry;
class URichTextBlock;
/**
 * 
 */
UCLASS()
class MUKSI_API UCardPreviewEffectPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCardData(UMuksiBattleCardDataAsset* InCardData);
	void ClearEffectData();
	
private:
	void EnsureEntryCount(int32 RequiredCount);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> EffectVerticalBox;

	UPROPERTY(EditDefaultsOnly, Category = "Card Preview")
	TSubclassOf<UCardPreviewEffectEntry> EffectEntryClass;

private:
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCardPreviewEffectEntry>> EffectEntries;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichText_DeceiveInfo;
};
