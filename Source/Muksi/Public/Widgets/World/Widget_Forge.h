// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_Forge.generated.h"

class UButton;

UCLASS()
class MUKSI_API UWidget_Forge : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RepairButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EnhanceButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	UFUNCTION()
	void HandleRepairButtonClicked();

	UFUNCTION()
	void HandleEnhanceButtonClicked();

	UFUNCTION()
	void HandleBackButtonClicked();

private:
	void RepairWeapon();
	void EnhanceWeapon();
};
