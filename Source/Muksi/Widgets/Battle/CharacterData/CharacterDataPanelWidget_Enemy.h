// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget.h"
#include "CharacterDataPanelWidget_Enemy.generated.h"

class UCharacterData_Enemy;
class UButton;
class UWidget_PlayerProfilePanel;
class UWidget_CharacterPassivePanel;
class ABattleCharacter_Enemy;
class UWidget_CharacterDeckPanel;
class UCommonAnimatedSwitcher;
/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterDataPanelWidget_Enemy : public UCharacterDataPanelWidget
{
	GENERATED_BODY()
	
public:
	void InitializeFromPlayerMode();
	void ApplyCharacterData(ABattleCharacter_Enemy* PlayerData);
	
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
protected:
	bool bCheckUI = false;	
	
protected:
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
	TObjectPtr<UWidget_PlayerProfilePanel> EnemyProfilePanelWidget;
	
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
	
	void SwitchEnemyPanel(int32 PanelIndex);
	
};
