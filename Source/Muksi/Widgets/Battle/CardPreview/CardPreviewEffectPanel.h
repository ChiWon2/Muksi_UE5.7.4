// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CardPreviewEffectPanel.generated.h"

class UVerticalBox;
class UScrollBox;

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
	
	//자동 스크롤---------------------------------------------------------------------------------------------------------
public:
	void StartAutoScroll();
	void StopAutoScroll();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_Effects;

	// 초당 이동하는 Slate 단위
	UPROPERTY(EditAnywhere, Category = "Preview|Scroll", meta = (ClampMin = "0.0"))
	float AutoScrollSpeed = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Preview|Scroll", meta = (ClampMin = "0.0"))
	float ScrollStartDelay = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = "Preview|Scroll", meta = (ClampMin = "0.0"))
	float ScrollEndDelay = 1.0f;

private:
	bool bAutoScrollActive = false;
	bool bWaitingAtBottom = false;
	float ScrollDelayRemaining = 0.0f;
	float AutoScrollOffset = 0.0f;
	//------------------------------------------------------------------------------------------------------------------
};
