// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_BattleSkillSlot.generated.h"

class UMuksiBattleCardDataAsset;
class UImage;
class UTextBlock;
class UButton;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBattleSkillSlotClicked, const FGuid&, UMuksiBattleCardDataAsset*);

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillSlotHovered, UMuksiBattleCardDataAsset*, int32);
DECLARE_MULTICAST_DELEGATE(FOnSkillSlotUnhovered);

UENUM(BlueprintType)
enum class ESkillEffectType : uint8
{
	None        UMETA(DisplayName = "None"),
	Strength    UMETA(DisplayName = "Strength"),
	ChangeEffect  UMETA(DisplayName = "ChangeEffect"),
};

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
	
	void SetCardInstance(const FGuid& InInstanceId,UMuksiBattleCardDataAsset* InCardData, int32 InRemainingCooldown, int32 InCurrentCost);
	
	FOnBattleSkillSlotClicked OnSkillSlotClicked;

	const FGuid& GetCardInstanceId() const{return CardInstanceId;}

	UMuksiBattleCardDataAsset* GetCardData() const{return CardData;}
	
	void ClearCardInstance();
	
	void SetEffectType(ESkillEffectType InEffectType);
	
	
	FOnSkillSlotHovered OnSkillSlotHovered;
	FOnSkillSlotUnhovered OnSkillSlotUnhovered;
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Skill;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_CooldownOverlay;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Effect;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TextBlock_SlotIndex = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> CooldownMaterialInstance;
	
	UPROPERTY(EditDefaultsOnly, Category = "Effect")
	ESkillEffectType EffectType = ESkillEffectType::None;
	
	void UpdateCooldownOverlay();
	
	UPROPERTY(EditDefaultsOnly, Category = "Battle|Skill")
	FLinearColor CooldownColor = FLinearColor(0.2f, 0.2f, 0.2f, 1.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Battle|Skill")
	FLinearColor NotEnoughCostColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f);
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Battle|Skill")
	FLinearColor StrengthEffectColor;
	
	UPROPERTY(EditDefaultsOnly, Category = "Battle|Skill")
	FLinearColor ChangeEffectColor;
	
	UPROPERTY(Transient)
	TObjectPtr<UMaterialInstanceDynamic> SlotMaterialInstance;

	void UpdateEffect();

private:
	UFUNCTION()
	void HandleSkillButtonClicked();
	
	UFUNCTION()
	void HandleSkillButtonHovered();

	UFUNCTION()
	void HandleSkillButtonUnhovered();
	
	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;
	
	UPROPERTY()
	int32 CurrentCost = 0;
	
	UPROPERTY(Transient)
	FGuid CardInstanceId;

	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;

	int32 RemainingCooldown = 0;
};
