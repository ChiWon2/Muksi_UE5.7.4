#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget_StatHUD.generated.h"

class UTextBlock;
class UStatComponent;

UCLASS()
class MUKSI_API UWidget_StatHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Stat HUD")
	void RefreshStatUI();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LightnessSkillText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InternalEnergyText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> VitalityText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SwordMasteryText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SpearMasteryText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HiddenWeaponMasteryText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FistMasteryText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SenseText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NegotiationText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HPText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> InternalEnergyResourceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> StaminaText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AttackPowerText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DefensePowerText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HitRateText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CriticalRateText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MoveSpeedText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> TravelMoveSpeedText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FateEncounterChanceText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FateGradeBonusText;

	UFUNCTION()
	void HandleStatChanged();

private:
	UPROPERTY()
	TObjectPtr<UStatComponent> CachedStatComponent = nullptr;

	UStatComponent* GetPlayerStatComponent();
	void SetTextIfValid(UTextBlock* TextBlock, const FText& Text);
};
