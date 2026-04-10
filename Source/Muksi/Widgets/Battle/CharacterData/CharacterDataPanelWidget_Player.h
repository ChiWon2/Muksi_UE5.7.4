// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Widgets/Battle/CharacterData/CharacterDataPanelWidget.h"
#include "CharacterDataPanelWidget_Player.generated.h"

class UCharacterDataBase;
class UWidget_CharacterDeckPanel;

/**
 * 
 */
UCLASS()
class MUKSI_API UCharacterDataPanelWidget_Player : public UCharacterDataPanelWidget
{
	GENERATED_BODY()
	
public:
	void InitializeFromPlayerMode();
	
	
	
	
protected:
	bool bCheckUI = false;	
	void ApplyCharacterData(UCharacterDataBase* InCharacterData);
	
	//***** BindWidget *****
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget_CharacterDeckPanel> CharacterDeckPanelWidget;
	//***** BindWidget *****
};
