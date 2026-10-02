// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actor_SkillInfoDescription.generated.h"

class UWidgetAnimation;

class UMuksiBattleCardDataAsset;
class UEffectRichTextBlock;

/**
 * 
 */
UCLASS()
class MUKSI_API UActor_SkillInfoDescription : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetCardData(UMuksiBattleCardDataAsset* CardData);
	
	void PlayShowAnimation();
	
protected:
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Show = nullptr;
	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEffectRichTextBlock> Text_EffectDescription = nullptr;
};
