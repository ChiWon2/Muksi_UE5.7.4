// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreviewPanel.generated.h"

class UCardPreview_ValuePanel;
class UTextBlock;
class UImage;
class UVerticalBox;


class UMuksiBattleCardDataAsset;
class UWidget_BattleCardBase;
class UCardPreviewEffectPanel;
class UCardPreview_ValuePanel;

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

	void HidePreview();
	void ShowPreview(UMuksiBattleCardDataAsset* InCardData);
protected:
	void SetCardData(UMuksiBattleCardDataAsset* InCardData);

	
	
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_CardName;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SkillCost;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SkillSpeed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SkillCoolDown;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CardIllustration;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardPreviewEffectPanel> CardPreviewEffectPanel;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_ValuePanels;

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Preview")
	TSubclassOf<UCardPreview_ValuePanel> ValuePanelClass;
	
private:
	UPROPERTY(Transient)
	TObjectPtr<const UMuksiBattleCardDataAsset> CurrentCardData;
	
	// 생성한 위젯을 보관해서 재사용
	UPROPERTY(Transient)
	TArray<TObjectPtr<UCardPreview_ValuePanel>> ValuePanels;

	void RefreshValuePanels(const UMuksiBattleCardDataAsset* CardData);
	
};
