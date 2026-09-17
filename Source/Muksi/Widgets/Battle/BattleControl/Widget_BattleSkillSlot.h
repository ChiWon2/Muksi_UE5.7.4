// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillSlot.generated.h"

class UMuksiBattleCardDataAsset;
class UImage;

class UButton;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillSlotClicked, const FGuid&, UMuksiBattleCardDataAsset*);
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleSkillSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetSlotIndex(int32 InSlotIndex);
	int32 GetSlotIndex() const { return SlotIndex; }
	
	void SetCardInstance(const FGuid& InInstanceId,UMuksiBattleCardDataAsset* InCardData);
	
	FOnBattleSkillSlotClicked OnSkillSlotClicked;

	const FGuid& GetCardInstanceId() const{return CardInstanceId;}

	UMuksiBattleCardDataAsset* GetCardData() const{return CardData;}
	
	void ClearCardInstance();
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Skill;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;


private:
	UFUNCTION()
	void HandleSkillButtonClicked();
	
	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;
	
	UPROPERTY(Transient)
	FGuid CardInstanceId;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;
};
