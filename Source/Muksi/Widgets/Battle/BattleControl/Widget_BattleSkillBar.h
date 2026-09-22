// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillBar.generated.h"

class UHorizontalBox;
class UWidget_BattleSkillSlot;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillSelected, const FGuid&, UMuksiBattleCardDataAsset*);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillHovered, UMuksiBattleCardDataAsset*, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleSkillUnhovered);
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleSkillBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBattleCharacter(ABattleCharacterBase* InCharacter);
	
	void SetSkillSlotActive(bool bActive);
	
	void RefreshSkillSlots();
	
	FOnBattleSkillSelected OnBattleSkillSelected;

	FOnBattleSkillHovered OnBattleSkillHovered;
	FOnBattleSkillUnhovered OnBattleSkillUnhovered;
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_SkillSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleSkillSlot>> SkillSlots;

private:
	void InitializeSkillSlots();
	void HandleSkillSlotClicked(const FGuid& InstanceId,UMuksiBattleCardDataAsset* CardData);
	
	void HandleSkillSlotHovered(UMuksiBattleCardDataAsset* SkillData,  int32 RemainingCooldown);
	void HandleSkillSlotUnhovered();
	
private:
	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> BoundCharacter = nullptr;
};
