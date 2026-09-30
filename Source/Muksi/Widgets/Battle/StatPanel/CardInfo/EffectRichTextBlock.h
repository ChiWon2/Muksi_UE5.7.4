// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/RichTextBlock.h"
#include "EffectRichTextBlock.generated.h"

USTRUCT(BlueprintType)
struct FEffectKeywordStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FSlateBrush IconBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FButtonStyle ButtonStyle;
};
/**
 * 
 */
class UEffectDescriptionPopup;
class UStatusEffectDefinitionDataAsset;

UCLASS()
class MUKSI_API UEffectRichTextBlock : public URichTextBlock
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Style")
	TMap<FName, FEffectKeywordStyle> KeywordStyles;
	
public:
	void ShowEffectDescriptionByHover(FName EffectID, const FEffectKeywordStyle& EffectStyle);
	
	void HideEffectDescriptionByHover();
	
	bool IsHoverPopupEnabled()const {return bEnableEffectHover;}
	
	void SetHoverPopupEnabled(bool bEnable) {bEnableEffectHover = bEnable;}
	float GetEffectIconScale() const
	{
		return EffectIconScale;
	}
protected:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Popup")
	TSubclassOf<UEffectDescriptionPopup> EffectDescriptionPopupClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect Rich Text")
	bool bEnableEffectHover = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect Style")
	float EffectIconScale = 1.0f;

private:
	UStatusEffectDefinitionDataAsset* FindStatusEffectDefinition(FName EffectID) const;

	void EnsureEffectDescriptionPopup();
	
	
	UPROPERTY()
	TObjectPtr<UEffectDescriptionPopup> EffectDescriptionPopup;
	

};
