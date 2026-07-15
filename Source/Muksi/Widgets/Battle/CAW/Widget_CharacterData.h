// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_CharacterData.generated.h"

class UCharacterDataPanelWidget_Enemy;
class UCharacterDataPanelWidget_Player;
class UButton;
class UCharacterDeckData;

class ABattleCharacterBase;

class ABattleCharacter_Player;
class ABattleCharacter_Enemy;

class UCharacterDataBase;
class UCharacterData_Player;
class UCharacterData_Enemy;
/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_CharacterData : public UWidget_ActivatableBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void CloseActivatableWidget();
	
	
protected:
	//~ Begin UserWidget Interface
	virtual void NativeConstruct() override;
	virtual void NativeOnDeactivated() override;
	//~ End UserWidget Interface
	
	
	//***** BindWidget *****	
	UPROPERTY(meta = (BindWidget))
	UButton* CloseBackgroundButton;
	
	UPROPERTY(meta = (BindWidget))
	UCharacterDataPanelWidget_Player* PlayerDataPanelWidget;
	UPROPERTY(meta = (BindWidget))
	UCharacterDataPanelWidget_Enemy* EnemyDataPanelWidget;
	//***** BindWidget *****	
	
	/*UPROPERTY()
	ABattleCharacter_Player* PlayerCharacterData;
	UPROPERTY()
	ABattleCharacter_Enemy* EnemyCharacterData;*/
	
	
	//Connect PlayerMode_Battle
public:
	void GetClickedActor(AActor* ClickedActor){SelectedActor = ClickedActor;};
	
	void GetCharacterData(ABattleCharacterBase* CharacterData);
protected:
	UPROPERTY()
	TObjectPtr<AActor> SelectedActor = nullptr;
	
	
	UPROPERTY()
	TObjectPtr<ABattleCharacter_Player> PlayerData = nullptr;
	UPROPERTY()
	TObjectPtr<ABattleCharacter_Enemy> EnemyData = nullptr;
	
	void SetWidgetVisible();
};
