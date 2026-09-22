// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillRevealPanel.generated.h"

class UWidget_BattleSkillRevealSlot;
class UMuksiBattleCardDataAsset;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSkillRevealFinished, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleSkillDeceiveRevealFinished);

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleSkillRevealPanel : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetExchangeSkills(
	int32 ExchangeIndex,
	UMuksiBattleCardDataAsset* FirstSkill,
	bool bFirstIsPlayer,
	UMuksiBattleCardDataAsset* SecondSkill,
	bool bSecondIsPlayer);
	
	void ClearAllSkills();
	
	bool RevealExchangeSkills(int32 ExchangeIndex, UMuksiBattleCardDataAsset* FirstSkill, 
		bool bFirstIsPlayer, UMuksiBattleCardDataAsset* SecondSkill, bool bSecondIsPlayer);

	FOnBattleSkillRevealFinished OnSkillRevealFinished;
	

	
protected:
	virtual void NativeConstruct() override;

private:
	void HandleSlotRevealFinished(UWidget_BattleSkillRevealSlot* FinishedSlot);

	int32 PendingRevealExchangeIndex = INDEX_NONE;
	int32 PendingRevealCount = 0;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange1_First;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange1_Second;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange2_First;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange2_Second;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange3_First;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_BattleSkillRevealSlot> Slot_Exchange3_Second;

	TArray<TObjectPtr<UWidget_BattleSkillRevealSlot>> FirstSlots;
	TArray<TObjectPtr<UWidget_BattleSkillRevealSlot>> SecondSlots;
	
	
		
	//변초 공개----------------------------------------------------------------------------------------------------------
public:
	bool PlayDeceiveReveal(
		int32 ExchangeIndex,
		bool bPlayerAction,
		UMuksiBattleCardDataAsset* ActualSkill);

	FOnBattleSkillDeceiveRevealFinished OnDeceiveRevealFinished;
private:
	UWidget_BattleSkillRevealSlot* FindActionSlot(int32 ExchangeIndex, bool bPlayerAction) const;
	//------------------------------------------------------------------------------------------------------------------
};
