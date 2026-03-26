// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerModeBase.generated.h"

class AMuksiPlayerController;
class UInputMappingContext;

/**
 * 
 */
UCLASS()
class MUKSI_API UPlayerModeBase : public UObject
{
	GENERATED_BODY()
		
public:
	virtual void EnterMode();
	virtual void ExitMode();
	virtual UInputMappingContext* GetInputMappingContext() const {return IMC;};
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC = nullptr;
};
