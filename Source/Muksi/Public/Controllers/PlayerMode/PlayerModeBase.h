// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InputActionValue.h"
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlayerModeBase.generated.h"

class AMuksiPlayerController;
class UInputMappingContext;
class ACineCameraActor;

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
	
	virtual void HandleLeftClick(const FInputActionValue& Value);
	virtual void HandleRightClick(const FInputActionValue& Value);
	
	virtual void HandlePPressedKey(const FInputActionValue& Value);
	virtual void HandleTabPressedKey(const FInputActionValue& Value);
	virtual void HandleIPressedKey(const FInputActionValue& Value);
	virtual void HandleEPressedKey(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void HandleQPressedKey(const FInputActionValue& Value);
	
	
	
	TObjectPtr<ACineCameraActor> ApplyStartCamera();
	
	//TObjectPtr<ACineCameraActor> GetViewCameraActor()const{return ModeViewCamera;};
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC = nullptr;
	
	UPROPERTY()
	TObjectPtr<AMuksiPlayerController> PC;
	
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<ACineCameraActor> ModeViewCamera;
	
	
	
};
