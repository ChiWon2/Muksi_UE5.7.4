// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PassiveActivePopupWidget.generated.h"

class ABattleCharacterBase;
class UVerticalBox;

class UPassiveActivePopup;

/**
 * 
 */
UCLASS()
class MUKSI_API UPassiveActivePopupWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox_Player;
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox_Enemy;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPassiveActivePopup> PassiveActivePopupClass_Player;
	UPROPERTY(EditAnywhere)
	TSubclassOf<UPassiveActivePopup> PassiveActivePopupClass_Enemy;
public:
	void SetData(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);
	UFUNCTION()
	void SetPassiveInfo_Player(UTexture2D* CharacterImage, FText PassiveName);
	UFUNCTION()
	void SetPassiveInfo_Enemy(UTexture2D* CharacterImage, FText PassiveName);
};
