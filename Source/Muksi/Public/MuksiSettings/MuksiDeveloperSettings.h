// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayTagContainer.h"
#include "MuksiDeveloperSettings.generated.h"

class UWidget_ActivatableBase;

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Muksi UI Settings"))
class MUKSI_API UMuksiDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(Config, EditAnywhere, Category = "Widget Reference", meta = (ForceInlineRow, Categories = "Muksi.Widget"))
	TMap<FGameplayTag, TSoftClassPtr<UWidget_ActivatableBase>> MuksiWidgetMap;
	
	UPROPERTY(Config, EditAnywhere, Category = "Options Image Reference", meta = (ForceInlineRow, Categories = "Frontend.Image"))
	TMap<FGameplayTag, TSoftObjectPtr<UTexture2D>> OptionsScreenSoftImageMap;
};
