// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "MuksiFunctionLibrary.generated.h"

class UPlayerModeBase;
class AMuksiPlayerController;
class UWidget_ActivatableBase;

/**
 * 
 */
UCLASS()
class MUKSI_API UMuksiFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, Category = "Muksi Function Library")
	static TSoftClassPtr<UWidget_ActivatableBase> GetMuksiSoftWidgetClassByTag(UPARAM(meta = (Categories = "Muksi.Widget"))FGameplayTag InWidgetTag);
	
	UFUNCTION(BlueprintPure, Category = "Frontend Function Library")
	static TSoftObjectPtr<UTexture2D> GetOptionsSoftImageByTag(UPARAM(meta = (Categories = "Frontend.Image")) FGameplayTag InImageTag);
	
	UFUNCTION(BlueprintPure, Category = "Muksi|PlayerController", meta=(WorldContext="WorldContextObject"))
	static AMuksiPlayerController* GetMuksiPlayerController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "Muksi|PlayerMode", meta=(WorldContext="WorldContextObject"))
	static UPlayerModeBase* GetCurrentMuksiPlayerMode(const UObject* WorldContextObject);
};
