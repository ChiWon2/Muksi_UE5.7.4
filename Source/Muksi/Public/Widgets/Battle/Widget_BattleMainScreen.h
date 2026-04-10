// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Widget_BattleMainScreen.generated.h"

class UWidget_CharacterData;
class UHandWidget;
class UInkLineWidget;

class UButton;

/**
 * 
 */
UCLASS()
class MUKSI_API UWidget_BattleMainScreen : public UWidget_ActivatableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Battle")
	void BP_OnSelectableCharacterClicked();
	

	
protected:
	//~Begin UCommonActivatableWidget Interface
	virtual void NativeConstruct() override;
	
	
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	
	virtual void NativeOnActivated() override;
	//~End UCommonActivatableWidget Interface
	
	
	
	//***** Bound Widgets ****
	UPROPERTY(meta = (BindWidget))
	UHandWidget* HandWidget;
	
	

	//***** Bound Widgets ****
	


};
