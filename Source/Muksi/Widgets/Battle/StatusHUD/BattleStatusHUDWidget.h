// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleStatusHUDWidget.generated.h"

class ABattleCharacterBase;
class UCharacterStatusWidget;
class UEffectDescriptionPopup;

/**
 * 
 */
UCLASS()
class MUKSI_API UBattleStatusHUDWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterStatusWidget> CharacterStatusWidget_Player;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCharacterStatusWidget> CharacterStatusWidget_Enemy;
	
	//스킬 호버링 코스트 보여주기-------------------------------------------------------------------------------------------
public:
	void SetPlayerCostPreview(int32 Cost);
	void ClearPlayerCostPreview();
	//------------------------------------------------------------------------------------------------------------------
};
