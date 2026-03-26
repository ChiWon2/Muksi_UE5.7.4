// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "MuksiGameplayTags.h"
#include "MuksiWorldSettings.generated.h"

/**
 * 
 */
UCLASS()
class MUKSI_API AMuksiWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta=(Categories = "Muksi.Widget"))
	FGameplayTag StartupWidgetTag;
	
	
	UFUNCTION(BlueprintCallable, Category = "Start Widget", DisplayName = "Get Start Widget Tag")
	FGameplayTag GetStartupWidgetTag() const;
};
