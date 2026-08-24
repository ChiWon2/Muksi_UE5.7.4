// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ExchangeControlWidget.generated.h"

class UCommonButtonBase;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndTurnRequested);
UCLASS()
class MUKSI_API UExchangeControlWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintAssignable, Category = "Exchange|Event")
	FOnEndTurnRequested OnEndTurnRequested;
	
	//CardSelect End Button------------------------------------------
public:
	void ShowSelectButton(bool bShow);
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonButtonBase> Button_Select;

private:
	void BindSelectButton();
	void UnbindSelectButton();

	void HandleSelectButtonClicked();
	//----------------------------------------------------------------
};
