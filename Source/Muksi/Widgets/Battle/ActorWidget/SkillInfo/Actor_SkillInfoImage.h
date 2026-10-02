// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Actor_SkillInfoImage.generated.h"

class UWidgetAnimation;
class UImage;
class UTextBlock;

class UMuksiBattleCardDataAsset;

/**
 * 
 */
UCLASS()
class MUKSI_API UActor_SkillInfoImage : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Skill = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Skill = nullptr;
	
	void SetCardData(UMuksiBattleCardDataAsset* InCardData);
	
	void PlayShowAnimation();
	
protected:
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_Show = nullptr;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;
};
