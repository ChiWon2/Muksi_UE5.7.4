// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterDataPanelWidget.generated.h"

class UButton;
class UCommonAnimatedSwitcher;
class UWidget_PlayerProfilePanel;
class ABattleCharacterBase;
class UWidget_CharacterDeckPanel;
class UWidget_CharacterPassivePanel;


UENUM()
enum class ECharacterDataPanel : uint8
{
	Profile,
	Skill,
	Passive
};

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterDataPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void ApplyCharacterData(ABattleCharacterBase* CharacterData);
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	

protected:
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonAnimatedSwitcher> WidgetSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Profile;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Skill;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Passive;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_PlayerProfilePanel> PlayerProfilePanelWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CharacterDeckPanel> CharacterDeckPanelWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CharacterPassivePanel> CharacterPassivePanelWidget;
	
	
protected:
	//Button Function
	UFUNCTION()
	void OnProfileButtonClicked();

	UFUNCTION()
	void OnSkillButtonClicked();

	UFUNCTION()
	void OnPassiveButtonClicked();

	void SwitchCharacterPanel(ECharacterDataPanel PanelType);
	void UpdateButtonState();

	void SetButtonSelected(UButton* Button, bool bSelected);
	
protected:
	bool bCheckUI = false;

	ECharacterDataPanel CurrentPanel = ECharacterDataPanel::Profile;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Button")
	FSlateBrush SelectedButtonBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Panel Button")
	FSlateBrush UnselectedButtonBrush;
};
