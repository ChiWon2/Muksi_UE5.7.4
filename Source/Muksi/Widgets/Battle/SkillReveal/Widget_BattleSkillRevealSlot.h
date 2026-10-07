// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillRevealSlot.generated.h"


class UImage;
class UWidgetAnimation;
class UMuksiBattleCardDataAsset;
class UMaterialInstanceDynamic;
class UWidget_BattleSkillRevealSlot;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillRevealAnimationFinished, UWidget_BattleSkillRevealSlot*);



DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillRevealSlotHovered, UMuksiBattleCardDataAsset*, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillRevealSlotUnhovered, bool);


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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	//BindWidget
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Skill;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> SkillData;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_SlotMaterial;
	
	// 변초가 BattleActionPresentation에서 공개되었는지
	bool bDeceiveRevealed = false;
	//BindWidget--------------------------------------------------------------------------------------------------------
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SlotMaterialInstance;
	
	UPROPERTY(EditDefaultsOnly, Category = "Reveal|Color")
	FLinearColor PlayerColor = FLinearColor(0.1f, 1.0f, 0.4f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Reveal|Color")
	FLinearColor EnemyColor = FLinearColor(1.0f, 0.1f, 0.1f, 1.0f);
	
	void UpdateOwnerColor();
	
	//슬롯 연출----------------------------------------------------------------------------------------------------------
public:
	void RevealDeceive(); //변초 공개
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_DissolveReveal;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DissolveMaterialInstance;
	bool bPlayingDissolveReveal = false;

	float DissolveElapsedTime = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Deceive Reveal")
	float DissolveDuration = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Deceive Reveal")
	float DissolveStartValue = 0.7f;

	UPROPERTY(EditAnywhere, Category = "Deceive Reveal")
	float DissolveEndValue = -0.47f;
	
	void FinishDeceiveReveal();

private:
	UFUNCTION()
	void HandleRevealAnimationFinished();
	
	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> Anim_Reveal;
	
	bool bIsPlayerSkill = false;
	//------------------------------------------------------------------------------------------------------------------
	
	//슬롯 Hover 정보띄우기-----------------------------------------------------------------------------------------------
public:
	FOnSkillRevealSlotHovered OnSkillHovered;
	FOnSkillRevealSlotUnhovered OnSkillUnhovered;
	
protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	//------------------------------------------------------------------------------------------------------------------
};
