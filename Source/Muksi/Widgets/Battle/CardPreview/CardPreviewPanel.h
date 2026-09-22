// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreviewPanel.generated.h"

class UTextBlock;
class UImage;

class UMuksiBattleCardDataAsset;
class UWidget_BattleCardBase;
class UCardPreviewEffectPanel;

/**
 * 
 */
UCLASS()
class MUKSI_API UCardPreviewPanel : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct()override;
	
	void HandleCardHovered(UWidget_BattleCardBase* CardWidget);
	void HandleCardHoverEnded(UWidget_BattleCardBase* CardWidget);
	
	
	void HandleSkillHovered(UMuksiBattleCardDataAsset* SkillData, int32 RemainingCooldown);
	void HandleSkillHoverEnded();

protected:
	void SetCardData(UMuksiBattleCardDataAsset* InCardData);

	void ShowPreview(UMuksiBattleCardDataAsset* InCardData);
	void HidePreview();
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CardName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CardSpeed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CardIllustration;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardPreviewEffectPanel> CardPreviewEffectPanel;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<const UMuksiBattleCardDataAsset> CurrentCardData;
	
};
