// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleControlPanel.generated.h"

class UWidget_BattleSkillBar;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillSelected, const FGuid&, UMuksiBattleCardDataAsset*);
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleControlPanel : public UUserWidget
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
	TObjectPtr<UWidget_BattleSkillBar> BattleSkillBar;
	
private:
	void HandleBattleSkillSelected(const FGuid& InstanceId, UMuksiBattleCardDataAsset* CardData);
};
