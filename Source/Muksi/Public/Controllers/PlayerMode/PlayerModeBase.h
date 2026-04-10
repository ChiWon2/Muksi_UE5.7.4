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
UCLASS(Abstract, Blueprintable, BlueprintType)
class MUKSI_API UPlayerModeBase : public UObject
{
	GENERATED_BODY()
		
public:
	virtual void EnterMode(AMuksiPlayerController* PlayerController);
	virtual void ExitMode();
	virtual UInputMappingContext* GetInputMappingContext() const {return IMC;};
	virtual int32 GetInputMappingPriority() const { return 0; }
	
	
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC = nullptr;
	
	UPROPERTY()
	TObjectPtr<AMuksiPlayerController> PC;
};
