#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_TrainingGrounds.generated.h"

class UButton;
class UStatComponent;

UCLASS()
class MUKSI_API UWidget_TrainingGrounds : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddLightnessSkillButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddInternalEnergyButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddVitalityButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddSwordMasteryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddSpearMasteryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddHiddenWeaponMasteryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddFistMasteryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddSenseButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddNegotiationButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void HandleAddLightnessSkillButtonClicked();

	UFUNCTION()
	void HandleAddInternalEnergyButtonClicked();

	UFUNCTION()
	void HandleAddVitalityButtonClicked();

	UFUNCTION()
	void HandleAddSwordMasteryButtonClicked();

	UFUNCTION()
	void HandleAddSpearMasteryButtonClicked();

	UFUNCTION()
	void HandleAddHiddenWeaponMasteryButtonClicked();

	UFUNCTION()
	void HandleAddFistMasteryButtonClicked();

	UFUNCTION()
	void HandleAddSenseButtonClicked();

	UFUNCTION()
	void HandleAddNegotiationButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();

private:
	UStatComponent* GetPlayerStatComponent() const;
};
