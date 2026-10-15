// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_CardPreviewCanvas.generated.h"

class UMuksiBattleCardDataAsset;
class UCardPreviewPanel;

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCardRevealPreviewFinished, int32);


UCLASS()
class MUKSI_API UWidget_CardPreviewCanvas : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	bool StartRevealPreview(int32 ExchangeIndex, UMuksiBattleCardDataAsset* FirstSkill, bool bFirstIsPlayer,
	   UMuksiBattleCardDataAsset* SecondSkill, bool bSecondIsPlayer);
	void StopRevealPreview();
	FOnCardRevealPreviewFinished OnRevealPreviewFinished;


	void ShowSkillPreview(UMuksiBattleCardDataAsset* SkillData, bool bIsPlayerSkill, int32 RemainingCooldown = -1);
	
	void HideSkillPreview(bool bIsPlayerSkill);
	void HideAllPreviews();
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardPreviewPanel> CardPreviewPanel_Player;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCardPreviewPanel> CardPreviewPanel_Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card Preview", meta = (ClampMin = "0.1"))
	float RevealPreviewDuration  = 2.0f;

private:
	void ShowNextRevealPreview();

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> FirstRevealSkill;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> SecondRevealSkill;

	bool bFirstRevealIsPlayer = false;
	bool bSecondRevealIsPlayer = false;
	bool bRevealPreviewRunning = false;

	int32 RevealPreviewStep = 0;
	int32 RevealExchangeIndex = INDEX_NONE;

	FTimerHandle RevealPreviewTimerHandle;
};
