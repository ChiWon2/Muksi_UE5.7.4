// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillRevealSlot.generated.h"


class UImage;
class UWidgetAnimation;
class UMuksiBattleCardDataAsset;
class UWidget_BattleSkillRevealSlot;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillRevealAnimationFinished, UWidget_BattleSkillRevealSlot*);

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleSkillRevealSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSkillData(UMuksiBattleCardDataAsset* InSkillData, bool IsPlayer);
	void ClearSkill();
	
	bool PlayRevealAnimation();

	FOnSkillRevealAnimationFinished OnRevealFinished;
	
	bool IsPlayerSkill() const
	{
		return bIsPlayerSkill;
	}
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Skill;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> SkillData;
	
	//슬롯 연출
private:
	UFUNCTION()
	void HandleRevealAnimationFinished();
	
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_Reveal;
	
	bool bIsPlayerSkill = false;
};
