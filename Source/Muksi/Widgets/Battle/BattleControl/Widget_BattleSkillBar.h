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
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleSkillBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetBattleCharacter(ABattleCharacterBase* InCharacter);
	
	void RefreshSkillSlots();
	
	FOnBattleSkillSelected OnBattleSkillSelected;
	
	bool ReturnCommittedSkill(const FGuid& InstanceId);
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_SkillSlots;

	UPROPERTY()
	TArray<TObjectPtr<UWidget_BattleSkillSlot>> SkillSlots;

	bool CommitSkillCard(const FGuid& InstanceId);
private:
	void InitializeSkillSlots();
	void HandleSkillSlotClicked(const FGuid& InstanceId,UMuksiBattleCardDataAsset* CardData);
	
private:
	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> BoundCharacter = nullptr;
};
