// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillRevealPanel.generated.h"

class UWidget_BattleSkillRevealSlot;
class UMuksiBattleCardDataAsset;

class UBorder;
class UOverlay;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSkillRevealFinished, int32);
DECLARE_MULTICAST_DELEGATE(FOnBattleSkillDeceiveRevealFinished);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillRevealHovered, UMuksiBattleCardDataAsset*, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBattleSkillRevealUnhovered, bool);
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
	bool PlayDeceiveReveal(int32 ExchangeIndex, bool bPlayerAction);

	FOnBattleSkillDeceiveRevealFinished OnDeceiveRevealFinished;
private:
	UWidget_BattleSkillRevealSlot* FindActionSlot(int32 ExchangeIndex, bool bPlayerAction) const;
	
	UPROPERTY(Transient)
	TObjectPtr<UWidget_BattleSkillRevealSlot> PendingDeceiveSlot;
	void HandleSlotDeceiveRevealFinished(UWidget_BattleSkillRevealSlot* FinishedSlot);
	//------------------------------------------------------------------------------------------------------------------
	
	//슬롯 Hover시 정보 띄우기---------------------------------------------------------------------------------------------
public:
	FOnBattleSkillRevealHovered OnSkillHovered;
	FOnBattleSkillRevealUnhovered OnSkillUnhovered;
	
private:
	void HandleSlotHovered(UMuksiBattleCardDataAsset* SkillData, bool bIsPlayerSkill);
	void HandleSlotUnhovered(bool bIsPlayerSkill);
	
	//------------------------------------------------------------------------------------------------------------------
	
	//SkillRevealSlot 아래로 내리기---------------------------------------------------------------------------------------
public:
protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UBorder> Border_HoverArea;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Slots;

	// 기본 위치에서 아래로 이동할 거리
	UPROPERTY(EditAnywhere, Category = "Reveal|Slide", meta = (ClampMin = "0.0"))
	float SlideDownDistance = 80.0f;

	// 초당 이동 거리
	UPROPERTY(EditAnywhere, Category = "Reveal|Slide", meta = (ClampMin = "1.0"))
	float SlideSpeed = 400.0f;

	// 내려온 위치에서 공개 연출을 유지할 시간
	UPROPERTY(EditAnywhere, Category = "Reveal|Slide", meta = (ClampMin = "0.0"))
	float RevealHoldDuration = 0.6f;

	FVector2D BaseSlotsTranslation = FVector2D::ZeroVector;

	bool bHoverAreaHovered = false;
	bool bRevealPresentationRunning = false;

	float RevealHoldElapsed = 0.0f;

	void TryFinishRevealPresentation();
	//------------------------------------------------------------------------------------------------------------------
	
};
