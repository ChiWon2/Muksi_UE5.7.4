// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget.h"
#include "CharacterDataPanelWidget_Player.generated.h"

class UWidget_CharacterPassivePanel;
class UWidget_PlayerProfilePanel;
class UCharacterDataBase;
class UWidget_CharacterDeckPanel;
class UCommonAnimatedSwitcher;
class ABattleCharacter_Player;
class UButton;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterDataPanelWidget_Player : public UCharacterDataPanelWidget
{
	GENERATED_BODY()
	
public:
	void InitializeFromPlayerMode();
	void ApplyCharacterData(ABattleCharacter_Player* PlayerData);
	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	bool bCheckUI = false;	
	
	
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonAnimatedSwitcher> WidgetSwitcher;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Profile;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Deck;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Passive;
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_PlayerProfilePanel> PlayerProfilePanelWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CharacterDeckPanel> CharacterDeckPanelWidget;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget_CharacterPassivePanel> CharacterPassivePanelWidget;
	//***** BindWidget *****
	
private:
	//Button Function
	UFUNCTION()
	void OnProfileButtonClicked();
	UFUNCTION()
	void OnDeckButtonClicked();
	UFUNCTION()
	void OnPassiveButtonClicked();
	
	void SwitchPlayerPanel(int32 PanelIndex);
};
