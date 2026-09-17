// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "EffectRichTextBlock.generated.h"

USTRUCT(BlueprintType)
struct FEffectKeywordStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
	FText DisplayName;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FButtonStyle ButtonStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};
/**
 * 
 */
class UEffectDescriptionPopup;


UCLASS()
class MUKSI_API UEffectRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Style")
	TMap<FName, FEffectKeywordStyle> KeywordStyles;
	
public:
	void ShowEffectDescription(const FText& EffectName, const FEffectKeywordStyle& EffectStyle);
	void HideEffectDescription();
	
	bool IsHoverPopupEnabled()const {return bEnableEffectHover;}
	
	void SetHoverPopupEnabled(bool bEnable) {bEnableEffectHover = bEnable;}
	float GetEffectIconScale() const
	{
		return EffectIconScale;
	}
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Popup")
	TSubclassOf<UEffectDescriptionPopup> EffectDescriptionPopupClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Rich Text")
	bool bEnableEffectHover = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Style")
	float EffectIconScale = 1.0f;

private:
	UPROPERTY()
	TObjectPtr<UEffectDescriptionPopup> EffectDescriptionPopup;
	

};
